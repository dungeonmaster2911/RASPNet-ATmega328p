#include "ISR_Clock_Data.h"
#include "../Layer2/DllFrame.h"
#include "../Utils/UsrUtils.h"
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