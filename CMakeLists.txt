# Basic CMake setup
cmake_minimum_required(VERSION 3.12)
file(WRITE ${CMAKE_BINARY_DIR}/CACHEDIR.TAG "Signature: 8a477f597d28d172789f06886806bc55")
include(PicoSDK.cmake)

# Define project
project(co2sensor C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
add_compile_options(-Wall -Wextra -Werror)

# Init SDK and define the executable
pico_sdk_init()
add_executable(co2sensor src/main.c)
target_link_libraries(co2sensor pico_stdlib)

# Enable USB and build map/bin/hex file etc.
pico_enable_stdio_usb(co2sensor 1)
pico_add_extra_outputs(co2sensor)
