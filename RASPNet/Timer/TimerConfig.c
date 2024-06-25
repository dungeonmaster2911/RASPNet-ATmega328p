/**
* @file TimerConfig.c
* @brief This file implements timer configuration necessary for RASPNet.
*/

#include "TimerConfig.h"
/**
* @brief Configure 16 bit timer1, for RASPNet clock
* 
* **Further Details**
*/
void timerInit(){

    /// - Set OCRA value for desired clock frequency
    //OCR1A = 62499; //frequency of 1 second
    //OCR1A = 31249; // frequency of 0.5 second
    //OCR1A = 15624; // frequency of 0.25 second
    //OCR1A = 6000;
    OCR1A = 1000; // frequency of 0.25 second

    /// - Set clock prescalar 256
    TCCR1B |= (1 << CS02);        
    //TCCR1B |= (1 << CS01);
    //TCCR1B |= (1 << CS00);

    TCCR1B |= (1 << WGM12);   /// - Enable CTC mode for timer1
    TIMSK1 |= (1 << OCIE1A);  /// - Enable timer compare interrupt
    TCNT1 = 0;                /// - Set Timer counter to 0
}
