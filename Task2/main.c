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
#include <util/atomic.h>


#include "RASPNetPinConfig.h"
#include "Utils/UsrUtils.h"
#include "UART/UARTSetup.h"
#include "Timer/TimerConfig.h"
#include "Layer2/DllFrame.h"
#include "Layer3/NetworkFrame.h"
#include "FrameQueue/Frame_Queue.h"



char* payload = "ABCD";
static bool block_send = false;


void handleRecveivedFrame(){
}

void checkInput(){
	static uint8_t serial_data = 0;
	static char* pd = "casex";
	uint8_t* frame = NULL;
	
	
	serial_data = serialRead(false);
	switch(serial_data){
		case '1':
			pd = "case1";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(1,1,frame);
			break;
		case '2':
		    pd = "case2";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(56,1,frame);
			break;
		case '3':
			pd = "case3";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(0,1,frame);
			break;
		case '4':
			pd = "case4";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(5,6,frame);
			break;
		case '5':
			pd = "case5";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(0,6,frame);
			break;
		case '6':
			// clear queue
			serialWriteStr("CLEAR QUEUE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			createFrameQueue();
			return;
			break;
		default:
			return;
			break;
	}
	addFrameToQueue(frame);
	
	//printFrame(frame);
}


int main(){
	createFrameQueue();
	sei();
	setIOPins();
	USART_Init();
	timerInit();
	serialWriteStr("--------------INFINITE LOOP ENTRY -----------");
	_delay_ms(2000);
	
	
	while(true){
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			checkInput();
		}
		/*
		for(int i=0;i<3;i++){
			char* serial_str = serialReadStr();
			serialWriteStr(serial_str);
			uint8_t str_len = strlen(serial_str);
			uint8_t* frame = createFrame((uint8_t*)serial_str,str_len);
			addNetworkHeaders(29,29,frame);
			
			addFrameToQueue(frame);
			serialWriteStr("Added frame to queue");
			
		}
		block_send = false;
		*/
		
		/*
		for(int i=0;i<4;i++){
			struct Node* node = getFrameFromQueue();
			serialWriteStr("Removed frame from queue");
			if(node == NULL){
				serialWriteStr("NULL!!");
				continue;
			}
			uint8_t* frame = node->frame;
			
			printFrame(frame);
			free(frame);
		}
		*/
		
		
		
		/*
		char* serial_str = serialReadStr();
		serialWriteStr(serial_str);
		uint8_t str_len = strlen(serial_str);
		
		uint8_t* frame = createFrame((uint8_t*)serial_str,str_len);
		addNetworkHeaders(5,45,frame);
		addFrameToQueue(frame);
		*/
		
		/*
		printFrame(frame);
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			copyFrameToSendBuffer(frame);
		}
		free(frame);
		*/
		
		
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
		
		/*
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
						crc32(crc_comp,pPayload(frame),*pSize(frame));
						
						
						char* string = (char*)malloc(40*sizeof(char));
						sprintf(string, "CRC_CALC=%lx   CRC_RECV=%lx",*((uint32_t*)crc_comp),*((uint32_t*)pCRC(frame)));
						serialWriteStr(string);
						free(string);
						
						
						if(*((uint32_t*)crc_comp) == *((uint32_t*)pCRC(frame))){
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
			
		}*/
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
		if(!block_send)
		sendDllFrame();
	}
	
	phase = !phase;
    
}

ISR(PCINT2_vect){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		recvDllFrame();
	}
}





