PRG            = $(shell find -name 'main.c' -exec basename {} .c \;)


MCU_TARGET     = atmega328p


OPTIMIZE       = -Os

CC             = avr-gcc

SRC = $(shell pwd)

# Override is only needed by avr-lib build system.
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump



all: dependency $(PRG).elf $(PRG).hex 

f : dependency $(PRG).elf $(PRG).hex flash

dependency :
	$(CC) $(CFLAGS) -c $(SRC)/Layer2/DllFrame.c -o DllFrame.o
	$(CC) $(CFLAGS) -c $(SRC)/UART/UARTSetup.c -o UARTSetup.o

$(PRG).elf: $(PRG).o RASPNetPinConfig.o UARTSetup.o Utils/UsrUtils.o Timer/TimerConfig.o DllFrame.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# dependency:
$(PRG).o: $(PRG).c RASPNetPinConfig.o UARTSetup.o Utils/UsrUtils.o Timer/TimerConfig.o DllFrame.o


$(PRG).hex: $(PRG).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

	
flash:
	sudo avrdude -c gpio -p $(MCU_TARGET) -v -U flash:w:./$(PRG).hex
	
clean:
	rm -rf *.o *.map *.hex *.elf
	
