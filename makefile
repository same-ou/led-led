MCU = atmega32u4
F_CPU = 16000000UL

# Compiler and linker settings
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -std=c99 -Wall -Wextra

# Source files
SRC = reciever.c ../common/usb.c

# Output files
OUT = main
HEX = $(OUT).hex

# Programming settings
PROGRAMMER = avr109
PORT = /dev/ttyACM0  # Change this to your Arduino's port

# Targets
all: $(HEX)

$(HEX): $(OUT).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(OUT).elf: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

flash: $(HEX)
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -U flash:w:$(HEX)

clean:
	rm -f $(OUT).elf $(HEX)

.PHONY: all flash clean