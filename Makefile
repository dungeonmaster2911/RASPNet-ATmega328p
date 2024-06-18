#PRG            = $(shell find -name 'main.c' -exec basename {} .c \;)
PRG            = main

MCU_TARGET     = atmega328p


OPTIMIZE       = -Os

CC             = avr-gcc

#SRC = $(shell pwd)
SRC = ./RASPNet

# Override is only needed by avr-lib build system.
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump



all: dependency $(PRG).elf $(PRG).hex 

f : dependency $(PRG).elf $(PRG).hex flash

dependency :
	$(CC) $(CFLAGS) -c $(SRC)/Layer2/DllFrame.c -o DllFrame.o
	$(CC) $(CFLAGS) -c $(SRC)/UART/UARTSetup.c -o UARTSetup.o
	$(CC) $(CFLAGS) -c $(SRC)/RASPNetPinConfig.c -o RASPNetPinConfig.o 
	$(CC) $(CFLAGS) -c $(SRC)/CRC/Custom_CRC32.c -o Custom_CRC32.o
	$(CC) $(CFLAGS) -c $(SRC)/Timer/TimerConfig.c -o TimerConfig.o 
	$(CC) $(CFLAGS) -c $(SRC)/Utils/UsrUtils.c -o UsrUtils.o
	$(CC) $(CFLAGS) -c $(SRC)/Layer3/NetworkFrame.c -o NetworkFrame.o
	$(CC) $(CFLAGS) -c $(SRC)/FrameQueue/Frame_Queue.c -o Frame_Queue.o

$(PRG).elf: $(PRG).o DllFrame.o UARTSetup.o RASPNetPinConfig.o Custom_CRC32.o TimerConfig.o UsrUtils.o NetworkFrame.o Frame_Queue.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 

# dependency:
$(PRG).o: $(PRG).c

$(PRG).hex: $(PRG).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

	
flash:
	sudo avrdude -c gpio -p $(MCU_TARGET) -v -U flash:w:./$(PRG).hex
	
clean:
	rm -rf *.o *.map *.hex *.elf

doc:
	doxygen DoxyConfig