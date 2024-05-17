#ifndef F_CPU
#define F_CPU 12000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "RASPNetPinConfig.h"
#include "Utils/UsrUtils.h"
#include "UART/UARTSetup.h"
#include "Timer/TimerConfig.h"
#include "Layer2/DllFrame.h"

char* payload = "ABCD";

int main(){
	sei();
	setIOPins();
	USART_Init();
	timerInit();
	serialWriteStr("--------------INFINITE LOOP ENTRY -----------");
	_delay_ms(2000);
	while(true){
		
		uint8_t * dll_frame = createDllFrame((uint8_t*)payload,strlen(payload));
		copyFrameToSendBuffer(dll_frame);
		//printFrameToSerial(dll_frame);
		_delay_ms(2000);
		free(dll_frame);
		dll_frame = NULL;
		//printFrameToSerial(dll_frame);
		
	}
	return 0;
}


ISR(TIMER1_COMPA_vect){
    
 
    controlBit(&PORTB,PORTB4,!bit_is_set(PINB,PINB4));
    //controlBit(&PORTB,PORTB5,!bit_is_set(PINB,PINB5));
    sendDllFrame();
}

ISR(PCINT2_vect){
    controlBit(&PORTB,PORTB0,bit_is_set(PIND,PIND5));
    recvDllFrame();
}
