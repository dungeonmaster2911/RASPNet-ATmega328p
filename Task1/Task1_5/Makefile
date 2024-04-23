PRG            = $(shell find -name '*.c' -exec basename {} .c \;)


MCU_TARGET     = atmega328p


OPTIMIZE       = -Os

CC             = avr-gcc

# Override is only needed by avr-lib build system.
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map
OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf $(PRG).hex flash

$(PRG).elf: $(PRG).o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# dependency:
$(PRG).o: $(PRG).c


$(PRG).hex: $(PRG).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@


	@echo "Press enter to continue flashing"
	@read tmp;
	
flash:
	sudo avrdude -c gpio -p $(MCU_TARGET) -v -U flash:w:./$(PRG).hex
	
