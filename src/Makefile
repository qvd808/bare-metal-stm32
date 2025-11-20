BUILD_DIR = build
TARGET    = $(BUILD_DIR)/blink

SRCS = src/main.c src/startup_stm32f446.s
OBJS = $(SRCS:src/%.c=$(BUILD_DIR)/%.o)
OBJS += $(SRCS:src/%.s=$(BUILD_DIR)/%.o)

all: $(TARGET).elf $(TARGET).bin

# C sources
$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assembly sources
$(BUILD_DIR)/%.o: src/%.s
	mkdir -p $(dir $@)
	$(AS) $(MCUFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin

flash: $(TARGET).elf
	openocd $(OPENOCD_PATHS) -c "program $(TARGET).elf verify reset exit"
