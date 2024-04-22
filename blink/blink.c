#define F_CPU 12000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

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


int main (void)
{
    DDRB |= _BV(PORTB0);      // set pin 0 of port B as an output pin
    DDRB |= _BV(PORTB1); 
    DDRB |= _BV(PORTB2);
    DDRB |= _BV(PORTB3);
    DDRB |= _BV(PORTB4);
    DDRB |= _BV(PORTB5);
    
    DDRD |= _BV(PORTD7);
    DDRD |= _BV(PORTD6);
    DDRD |= _BV(PORTD5);
    DDRD |= _BV(PORTD4);
    
    bool a;
    //PORTD = 0b11111111;
    PORTD |= _BV(PORTD7);
    _delay_ms(1000);
    //PORTD = 0b00000000;
    PORTD &= ~_BV(PORTD7);
    _delay_ms(1000);

    for (;;) {
        led_control(0,0,0,0,0,0,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,0,0,0,0,0,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,0,0,0,0,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,0,0,0,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,1,0,0,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,1,1,0,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,1,1,1,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,1,1,1,1,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,1,1,1,1,1,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,1,1,1,1,1,1,0);
        _delay_ms(500);
        //----------------------------------------------
        led_control(1,1,1,1,1,1,1,1,1,1);
        _delay_ms(500);
        //---------------------------------------------------------
        led_control(1,1,1,1,1,1,1,1,1,0);
        _delay_ms(500);
       
        led_control(1,1,1,1,1,1,1,1,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,1,1,1,1,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,1,1,1,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,1,1,0,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,1,0,0,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,1,0,0,0,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,1,0,0,0,0,0,0,0,0);
        _delay_ms(500);
        
        led_control(1,0,0,0,0,0,0,0,0,0);
        _delay_ms(500);
        /*
        PORTB = 0b00000001;
        
        _delay_ms(500);
        
        PORTB = 0b00000010;
        
        _delay_ms(500);
        
        PORTB = 0b00000100;
        
        _delay_ms(500);
        
        PORTB = 0b00001000;
        
        _delay_ms(500);
        
        PORTB = 0b00010000;
        
        _delay_ms(500);
        
        PORTB = 0b00100000;
        
        
        _delay_ms(500);
        
        
        led_control(0,0,0,1,0,0,0,0,0,0);
        
        _delay_ms(1000);
         */
       // PORTB |= _BV(PORTB0);  // set pin 0 of port B high
        //_delay_loop_2(62500*2);  // loop for 62500 iterations * 4 cycles / 1MHz clock ~= 250ms
        //PORTB &= ~_BV(PORTB0); // set pin 0 of port B low
        //_delay_loop_2(62500*2);  // loop for 62500 iterations * 4 cycles / 1MHz clock ~= 250ms
    }
}
