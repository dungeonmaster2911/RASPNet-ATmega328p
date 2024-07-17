/**
* @file ISR_Clock_Data.c
* @brief This file implements functionalities of the Layer 1
*/

#include "ISR_Clock_Data.h"
#include "../Layer2/DllFrame.h"
#include "../Utils/UsrUtils.h"


static bool enable_comm = true;

/**
* @brief Layer 1 -> Toggle Clock and Send Data
* 
* **Further Details**
*/
ISR(TIMER1_COMPA_vect){
    if(!enable_comm){return;}
    /// Using bool `phase` to implement phase shift (basically divides the timer clock by 2)
    static bool phase = false;
    
    /// If pahse is TRUE -> Toggle Clock Pin
    if(phase){
		controlBit(&PORTB,PORTB4,!bit_is_set(PINB,PINB4));
		controlBit(&PORTB,PORTB0,bit_is_set(PIND,PIND4));
	}
	/// If pahse is FALSE -> Send 1 data bit
	else{
		sendDllFrame();
	}
	/// Toggle phase variable
	phase = !phase;
}

/**
* @brief Layer 1 -> ISR to receive Data 
* 
* **Further Details**
*/
ISR(PCINT2_vect){
    if(!enable_comm){return;}
    ///Call receive function
    recvDllFrame();
}


/**
* @brief Function to enable/disable communication 
*/
void setCommEnable(bool enable){
    enable_comm = enable;
}
