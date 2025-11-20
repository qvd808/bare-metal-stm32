# Output directory
BUILD_DIR = build

# Target name
TARGET = $(BUILD_DIR)/blink

# Source files
SRCS = src/main.c \
       src/startup.c

# Object files
OBJS = $(SRCS:src/%.c=$(BUILD_DIR)/%.o)

# Toolchain
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy

# MCU settings (adjust if using another F4 chip)
MCUFLAGS = -mcpu=cortex-m4 -mthumb

# Compiler flags
CFLAGS  = $(MCUFLAGS) -O2 -g -ffreestanding -fno-builtin -Wall -Wextra

# Linker flags
LDFLAGS = $(MCUFLAGS) -T linker_script.ld -nostartfiles -Wl,--gc-sections

all: $(TARGET).elf $(TARGET).bin

# C source → .o
$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link ELF
$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

# ELF → BIN
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Flash target (optional)
flash: $(TARGET).elf
	openocd -f dev_board.cfg -c "program $(TARGET).elf verify reset exit"
