#include "TimerConfig.h"
void timerInit(){
    //OCR1A = 62499; // frequency of 1 second
    //OCR1A = 31249; // frequency of 0.5 second
    OCR1A = 15624; // frequency of 0.25 second
    //OCR1A = 5; // frequency of 0.25 second
    TCCR1B |= (1 << CS02);        // prescalar 256
    //TCCR1B |= (1 << CS01);
    //TCCR1B |= (1 << CS00);
    TCCR1B |= (1 << WGM12);   // turns on CTC mode for timer1
    TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
    TCNT1 = 0;                // 16 bit register
}
