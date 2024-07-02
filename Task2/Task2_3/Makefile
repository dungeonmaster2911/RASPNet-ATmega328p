PRG            = $(shell find -name '*.c' -exec basename {} .c \;)


MCU_TARGET     = atmega328p


OPTIMIZE       = -Os

CC             = avr-gcc

# Override is only needed by avr-lib build system.
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf $(PRG).hex

f : $(PRG).elf $(PRG).hex flash

$(PRG).elf: $(PRG).o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# dependency:
$(PRG).o: $(PRG).c


$(PRG).hex: $(PRG).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

	
flash:
	sudo avrdude -c gpio -p $(MCU_TARGET) -v -U flash:w:./$(PRG).hex
	
clean:
	rm -rf *.o *.map *.hex *.elf
	
