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
#include <util/atomic.h>


char* payload = "ABCD";

void loop(){
	while(true){

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
		
		
		
		char* serial_str = serialReadStr();
		serialWriteStr(serial_str);
		uint8_t str_len = strlen(serial_str);
		
		uint8_t* frame = createFrame((uint8_t*)serial_str,str_len);
		addNetworkHeaders(29,29,frame);
		
		printFrame(frame);
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			copyFrameToSendBuffer(frame);
		}
		free(frame);
		
		/*
		uint8_t* network_frame = addNetworkHeaders(123,234,(uint8_t*)serial_str,&str_len);
		
		//uint8_t * dll_frame = createDllFrame((uint8_t*)serial_str,strlen(serial_str));
		uint8_t * dll_frame = createDllFrame(network_frame,str_len);
		
		copyFrameToSendBuffer(dll_frame);
		free(serial_str);
		free(dll_frame);
		free(network_frame);
		
		dll_frame = NULL;
		network_frame = NULL;
		*/
		while(true){
			uint8_t* frame = getRecvFrame();
			static bool forward = false;
			 
			if(frame == NULL){
				serialWriteStr("NO frame to read");
			}
			else{
				switch(checkDestination(frame)){
					case 0:serialWriteStr("BROADCAST");
						forward = false;
						break;
					case 1:serialWriteStr("SELF");
						uint8_t* crc_comp = (uint8_t*)malloc(4*sizeof(uint8_t));						
						crc32(crc_comp,pPayload(frame),pSize(frame));
						if(*((uint32_t*)crc_comp) == *pCRC(frame)){
							printFrame(frame);
						}
						else{
							serialWriteStr("CRC error");
						}
						forward = false;
						break;
					case 2:serialWriteStr("FORWARD");
						printFrame(frame);
						copyFrameToSendBuffer(frame);
						forward = true;
						break;
					default:serialWriteStr("ERROR");
						break;
				}
				free(frame);
				if(!forward)
				break;
			}
			_delay_ms(2000);
		}
    }
	return 0;
}



ISR(TIMER1_COMPA_vect){
    //Variable to implement phase shift
    static bool phase = false;
    
    //basically divides the timer clock by 2
    if(phase){
		controlBit(&PORTB,PORTB4,!bit_is_set(PINB,PINB4));
		controlBit(&PORTB,PORTB0,bit_is_set(PIND,PIND4));
		
	}
	else{
		sendDllFrame();
	}
	
	phase = !phase;
    
    //controlBit(&PORTB,PORTB5,!bit_is_set(PINB,PINB5));
    
    //sendDllFrame();
    //controlBit(&PORTB,PORTB4,!bit_is_set(PINB,PINB4));
    //controlBit(&PORTB,PORTB0,bit_is_set(PIND,PIND4));
}

ISR(PCINT2_vect){
    //controlBit(&PORTB,PORTB0,bit_is_set(PIND,PIND5));
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		recvDllFrame();
	}
}





