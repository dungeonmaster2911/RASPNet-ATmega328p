#define F_CPU 12000000UL // Defining the CPU Frequency

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>

// variables for program logic

uint8_t time_ct = 0;
uint8_t usr_delay = 20;
bool B5 = true;
bool D4 = true;
uint8_t seq = 0;
bool dir = true;

// supporting functions for led control

void controlBit(volatile uint8_t* reg,uint8_t pin,bool val){
        if(val){
                *reg |= _BV(pin);
        }
        else{
                *reg &= ~_BV(pin);
        }
}

void led_control(bool b3,bool b4,bool b5,bool b6,bool b7,bool b8,
        bool b9,bool b10,bool b11,bool b12){

        controlBit(&PORTD,PORTD4,b3);
        controlBit(&PORTD,PORTD5,b4);
        controlBit(&PORTD,PORTD6,b5);
        controlBit(&PORTD,PORTD7,b6);
        controlBit(&PORTB,PORTB0,b7);
        controlBit(&PORTB,PORTB1,b8);
        controlBit(&PORTB,PORTB2,b9);
        controlBit(&PORTB,PORTB3,b10);
        controlBit(&PORTB,PORTB4,b11);
        controlBit(&PORTB,PORTB5,b12);

}

void led_sequence_1(){
        static uint8_t seq = 0;
        static bool dir = true;
        
        switch(seq){
        case 0:{
                led_control(0,0,0,0,0,0,0,0,0,0);
                break;
        }
        
case 1:{
        led_control(1,0,0,0,0,0,0,0,0,0);
        break;
}

case 2:{
        led_control(1,1,0,0,0,0,0,0,0,0);
        break;
}

case 3:{
        led_control(1,1,1,0,0,0,0,0,0,0);
        break;
}

case 4:{
        led_control(1,1,1,1,0,0,0,0,0,0);
        break;
}

case 5:{
        led_control(1,1,1,1,1,0,0,0,0,0);
        break;
}

case 6:{
        led_control(1,1,1,1,1,1,0,0,0,0);
        break;
}

case 7:{
        led_control(1,1,1,1,1,1,1,0,0,0);
        break;
}

case 8:{
        led_control(1,1,1,1,1,1,1,1,0,0);
        break;
}

case 9:{
        led_control(1,1,1,1,1,1,1,1,1,0);
        break;
}

case 10:{
        led_control(1,1,1,1,1,1,1,1,1,1);
        break;
}
default:break;
}


if (dir)
{
        seq++;
} else {
        seq --;
}
if(seq == 10 || seq == 0){
        dir = !dir;
}
}

// timer setup

void settimer(){

    OCR1A = 1874;                   // One tick is used to return to zero

    TCCR1B |= (1 << CS01);        // prescalar 64
    TCCR1B |= (1 << CS00);

    TCCR1B |= (1 << WGM12);   // turns on CTC mode for timer1

    

    TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

    TCNT1 = 0;                // 16 bit register
}

void pinsetup(){
    DDRB |= _BV(PORTB0);
    DDRB |= _BV(PORTB1);
    DDRB |= _BV(PORTB2);
    DDRB |= _BV(PORTB3);
    DDRB |= _BV(PORTB4);
    DDRB |= _BV(PORTB5);
    
    DDRD |= _BV(PORTD7);
    DDRD |= _BV(PORTD6);
    DDRD |= _BV(PORTD5);
    DDRD |= _BV(PORTD4);
}

int main() {
    pinsetup();
    sei();
    settimer();
    while (1){

    }
    return 0;
}


ISR(TIMER1_COMPA_vect){
        time_ct++;

        if(time_ct >= usr_delay){
                led_sequence_1();
                time_ct = 0; 
        }
}
