    /**
* @file RASPNetPinConfig.c
* @brief This file implements pins configuration of the ATMEGA328p. 
*/

#include "RASPNetPinConfig.h"
#include "Utils/UsrUtils.h"

/**
*  @brief Setup I/O pins of the ATMEGA328p 
* 
* **Further Details**
*/
void setIOPins(){
    
    controlBit(&DDRB,PORTB0,1); /// - PB0: LED Indicator for Clock -> OUTPUT
    controlBit(&DDRB,PORTB4,1); /// - PB4: Clock send -> OUTPUT
    controlBit(&DDRB,PORTB5,1); /// - PB5: Data send -> OUTPUT
    controlBit(&DDRD,PORTD4,0); /// - PD4: Clock receive -> INPUT
    controlBit(&DDRD,PORTD5,0); /// - PD5: Data receive -> INPUT

    
    /** 
     * - Enable interrupt for Clock receive PD4 \n
     *   * PD4->PCINT20 \n
     *   * PCINT20->PCMSK2 \n
     *   * PCMSK2-> PCICR->PCIE2 \n
    */
    controlBit(&PCICR,PCIE2,1);
    controlBit(&PCMSK2,PCINT20,1); ///
}
