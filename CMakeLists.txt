cmake_minimum_required(VERSION 3.15)

# Set toolchain before project()
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Prevent CMake from testing the compiler
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)

project(blink C ASM)

set(EXECUTABLE ${CMAKE_PROJECT_NAME})

# MCU settings
set(CPU_PARAMETERS
    -mcpu=cortex-m4
    -mthumb
)

# Compiler flags (matching your Makefile's CFLAGS)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -ffreestanding -fno-builtin -Wall -Wextra")

# Linker script
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker_script.ld)

# Include directories
set(INCS
    cmsis/device/include
    cmsis/core
)

# Source files
set(SRCS
    src/main.c
    src/startup.c
	cmsis/device/source/system_stm32f4xx.c
)

# Create executable
add_executable(${EXECUTABLE}.elf
    ${SRCS})

# Add header directories
target_include_directories(${EXECUTABLE}.elf PRIVATE
    ${INCS})

# Embedded macros (if needed)
target_compile_definitions(${EXECUTABLE}.elf PRIVATE
    STM32F446xx)

# Compiler options
target_compile_options(${EXECUTABLE}.elf PRIVATE
    ${CPU_PARAMETERS})

# Linker options: use linker script, no standard startup files, remove unused sections
target_link_options(${EXECUTABLE}.elf PRIVATE
    ${CPU_PARAMETERS}
    -T${LINKER_SCRIPT}
    -nostartfiles
    -Wl,--gc-sections
)

# Generate .bin file from .elf
add_custom_command(TARGET ${EXECUTABLE}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary 
    $<TARGET_FILE:${EXECUTABLE}.elf> 
    ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.bin
    COMMENT "Building ${EXECUTABLE}.bin")

# Optional: Flash target using OpenOCD
add_custom_target(flash
    COMMAND openocd -f ${CMAKE_CURRENT_SOURCE_DIR}/dev_board.cfg 
    -c "program ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.elf verify reset exit"
    DEPENDS ${EXECUTABLE}.elf
    COMMENT "Flashing ${EXECUTABLE}.elf to board")
