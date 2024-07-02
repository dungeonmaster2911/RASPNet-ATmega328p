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
#include "PiAtmegaAPI/PiAtmegaAPI.h"


char* payload = "ABCD";



void handleRecveivedFrame(){
	uint8_t* recv_frame;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		recv_frame = getRecvFrame();
	}

	if(recv_frame){
		processNetworkFrame(recv_frame);
		if(recv_frame){
			serialWriteStr("Frame processed by Layer 3");
			free(recv_frame);
		}
		else{
			serialWriteStr("Frame discarded by Layer 3")
		}
	}
	

	/*
	if(recv_frame !=NULL){
		printFrame(recv_frame);
		free(recv_frame);
		recv_frame = NULL;
	}
	*/	
}

void checkInput(){
	char* serial_str = serialReadStr();
	if(serial_str){
		parseCmdFromPi(serial_str);
		free(serial_str);
	}
}


int main(){
	createFrameQueue();
	sei();
	setIOPins();
	USART_Init();
	timerInit();
	serialWriteStr("-------------RASPNET-----------");
	_delay_ms(2000);
	
	
	while(true){
		
		
		checkInput();
		handleRecveivedFrame();
    }
	return 0;
}


/**
* @brief Layer 1 Clock
* 
* **Further Details**
*/
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
    
}

/**
* @brief Layer 1 Data
* 
* **Further Details**
*/
ISR(PCINT2_vect){
	recvDllFrame();
}





