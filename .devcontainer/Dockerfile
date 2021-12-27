# See here for image contents: https://github.com/microsoft/vscode-dev-containers/blob/main/containers/ubuntu/.devcontainer/base.Dockerfile
FROM mcr.microsoft.com/vscode/devcontainers/base:ubuntu

# Update OS
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update \
    && apt-get upgrade --yes

# Install required packages
ENV PACKAGE_LIST build-essential \
    pkg-config \
    cmake \
    gcc-arm-none-eabi \
    libusb-1.0 \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib
RUN apt-get update \
    && apt-get install --yes --no-install-recommends ${PACKAGE_LIST}

# Install pico SDK
ENV TOOLCHAINS /etc/toolchains
ENV PICO_SDK_PATH ${TOOLCHAINS}/pico/pico-sdk
RUN mkdir -p ${TOOLCHAINS} \
    && cd ${TOOLCHAINS} \
    && wget https://raw.githubusercontent.com/raspberrypi/pico-setup/master/pico_setup.sh -O ./pico_setup.sh \
    && chmod +x ./pico_setup.sh \
    && SKIP_UART=1 SKIP_VSCODE=1 SKIP_OPENOCD=1 ./pico_setup.sh
