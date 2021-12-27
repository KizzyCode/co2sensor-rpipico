#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"


// Define how the MH-Z19C sensor is connected
#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 12
#define UART_RX_PIN 13


/// Computes a MH Z19C compatible checksum
uint8_t mh_z19c_csum(const uint8_t msg[8]) {
    // Return a faulty checksum if the message is invalid
    if (msg[0] != 0xFF) {
        return 0x00;
    }

    // Compute the checksum
    uint8_t csum = 0xFF - (msg[1] + msg[2] + msg[3] + msg[4] + msg[5] + msg[6] + msg[7]);
    return csum + 1;
}
/// Writes a message to the sensor
void mh_z19c_request(const uint8_t msg[8]) {
    // Compute the checksum and write the command
    uint8_t csum = mh_z19c_csum(msg);
    uart_write_blocking(UART_ID, msg, 8);
    uart_write_blocking(UART_ID, &csum, 1);
}
/// Reads a response for a given command from the sensor
int mh_z19c_response(uint8_t cmd, uint8_t msg[8]) {
    // Scan for response begin and let the LED flicker during scan
    while (!(msg[0] == 0xFF && msg[1] == cmd)) {
        // Shift the array one to the left
        msg[0] = msg[1];
        
        // Read the next byte
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        uart_read_blocking(UART_ID, &msg[1], 1);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
    }

    // Read the remaining message
    uint8_t csum = 0;
    uart_read_blocking(UART_ID, &msg[2], 6);
    uart_read_blocking(UART_ID, &csum, 1);
    
    // Validate the checksum
    uint8_t csum_real = mh_z19c_csum(msg);
    return csum == csum_real ? 0 : -1;
}


/// Enables/disables the autocalibration of the sensor
void mh_z19c_autocalibration(bool enable) {
    uint8_t payload = enable ? 0xA0 : 0x00;
    uint8_t cmd[8] = { 0xFF, 0x01, 0x79, payload, 0x00, 0x00, 0x00, 0x00 };
    mh_z19c_request(cmd);
}
/// Reads the CO2 concentration from the sensor
int mh_z19c_readco2(uint16_t* ppm) {
    // Send the read command and give the sensor some time
    uint8_t cmd[8] = { 0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00 };
    mh_z19c_request(cmd);

    // Read the response
    uint8_t response[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    if (mh_z19c_response(0x86, response) != 0) {
        return -1;
    }

    // Parse the response
    *ppm = ((uint16_t)response[2] * 256) + (uint16_t)response[3];
    return 0;
}


int main() {
    // Initialize USB communication
    stdio_usb_init();

    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Enable autocalibration and enter the runloop
    mh_z19c_autocalibration(true);
    while (true) {
        // Read the value
        uint16_t ppm = 0;
        if (mh_z19c_readco2(&ppm) == 0) {
            printf("{ \"type\": \"co2_ppm\", \"value\": %d }\n", ppm);
        } else {
            printf("{ \"type\": \"co2_ppm\", \"error\": \"invalid_checksum\" }\n");
        }
        
        // Sleep 5s
        sleep_ms(5000);
    }
    return 0;
}
