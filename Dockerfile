FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# Install build deps + cmake + ncurses for gdb
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates \
        wget \
        cmake \
        make \
        build-essential \
        libncurses5 \
        libncurses5-dev \
        libtinfo5 \
        xz-utils && \
    rm -rf /var/lib/apt/lists/*

# Install ARM GNU toolchain
RUN cd /opt && \
    wget -q "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2" && \
    tar -xjf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 && \
    rm gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2

ENV PATH="/opt/gcc-arm-none-eabi-10.3-2021.10/bin:${PATH}"

# Work in mounted project directory
WORKDIR /app

# Default command: configure and build with CMake
CMD ["sh", "-c", "cmake -B build -S . && cmake --build build"]
