#include "RASPNetPinConfig.h"
#include "Utils/UsrUtils.h"


void setIOPins(){
    /*
        PB4: Clock send -> OUTPUT
        PB5: Data send -> OUTPUT
        PD4: Clock receive -> INPUT
        PD5: Data receive -> INPUT
        PB4
    */
    controlBit(&DDRB,PORTB0,1);
    controlBit(&DDRB,PORTB4,1);
    controlBit(&DDRB,PORTB5,1);
    controlBit(&DDRD,PORTD4,0);
    //controlBit(&PORTD,PORTD4,0);
    controlBit(&DDRD,PORTD5,0);
    //controlBit(&PORTD,PORTD5,0);
    //DDRD = 0x00; 
    //DDRB |= 1<<PORTB4;
    //DDRB |= 1<<PORTB5;
    
    /* 
     * Enable interrupt for Clock receive PD4
     * PD4->PCINT20
     * PCINT20->PCMSK2
     * PCMSK2-> PCICR->PCIE2
    */
    controlBit(&PCICR,PCIE2,1);
    controlBit(&PCMSK2,PCINT20,1);
}
