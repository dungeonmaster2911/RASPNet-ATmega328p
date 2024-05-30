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
#include "Layer3/NetworkFrame.h"



char* payload = "ABCD";

void wtf(){
	while(true){
		int a = 66* 67;
		a++;
		}
}

int main(){
	sei();
	setIOPins();
	USART_Init();
	timerInit();
	serialWriteStr("--------------INFINITE LOOP ENTRY -----------");
	_delay_ms(2000);
	while(true){
		
		//uint8_t * dll_frame = createDllFrame((uint8_t*)payload,strlen(payload));
		//copyFrameToSendBuffer(dll_frame);
		
		char* serial_str = serialReadStr();
		serialWriteStr(serial_str);
		uint8_t str_len = strlen(serial_str);
		
		
		uint8_t* network_frame = addNetworkHeaders(123,234,(uint8_t*)serial_str,&str_len);
		
		//uint8_t * dll_frame = createDllFrame((uint8_t*)serial_str,strlen(serial_str));
		uint8_t * dll_frame = createDllFrame(network_frame,str_len);
		
		copyFrameToSendBuffer(dll_frame);
		free(serial_str);
		free(dll_frame);
		free(network_frame);
		
		dll_frame = NULL;
		network_frame = NULL;
		//printFrameToSerial(dll_frame);
		//_delay_ms(2000);
		//_delay_ms(2000);
		//_delay_ms(2000);
		
		//free(dll_frame);
		//dll_frame = NULL;
		
		//printFrameToSerial(dll_frame);
		
		/*
		char* string = (char*)malloc(40*sizeof(char));
        sprintf(string, "Char=%d",serialRead(true));
        serialWriteStr(string);
        free(string);
        */
        
        //Try to read a received frame if available
        //while(true){
		//}
        //wtf();
		
	}
	return 0;
}



ISR(TIMER1_COMPA_vect){
    
 
    
    //controlBit(&PORTB,PORTB5,!bit_is_set(PINB,PINB5));
    
    sendDllFrame();
    controlBit(&PORTB,PORTB4,!bit_is_set(PINB,PINB4));
    controlBit(&PORTB,PORTB0,bit_is_set(PIND,PIND4));
}

ISR(PCINT2_vect){
    //controlBit(&PORTB,PORTB0,bit_is_set(PIND,PIND5));
    recvDllFrame();
}





