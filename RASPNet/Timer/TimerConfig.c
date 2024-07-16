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

    /// - Set OCRA value for desired timer frequency
    /// - RASPNet Clock = 2 * Timer (Due to phase shift logic)
    //OCR1A = 62499; //timer of 1 seconds
    //OCR1A = 31249; // timer of 0.5 seconds
    //OCR1A = 15624; // timer of 0.25 seconds
    //OCR1A = 6000;
    OCR1A = 1400; /// - Timer of 0.30 seconds (Communication Clock = 2*Timer)

    /// - Set clock prescalar 256
    TCCR1B |= (1 << CS02);        
    //TCCR1B |= (1 << CS01);
    //TCCR1B |= (1 << CS00);

    TCCR1B |= (1 << WGM12);   /// - Enable CTC mode for timer1
    TIMSK1 |= (1 << OCIE1A);  /// - Enable timer compare interrupt
    TCNT1 = 0;                /// - Set Timer counter to 0
}
