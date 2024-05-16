#define F_CPU 12000000UL // Defining the CPU Frequency

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

// Defining Necessary macros for USART 
#define USART_BAUDRATE 9600 // Desired Baud Rate
#define BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection

#define DISABLED    (0<<UPM00)
#define EVEN_PARITY (2<<UPM00)
#define ODD_PARITY  (3<<UPM00)
#define PARITY_MODE  DISABLED // USART Parity Bit Selection

#define ONE_BIT (0<<USBS0)
#define TWO_BIT (1<<USBS0)
#define STOP_BIT ONE_BIT      // USART Stop Bit Selection

#define FIVE_BIT  (0<<UCSZ00)
#define SIX_BIT   (1<<UCSZ00)
#define SEVEN_BIT (2<<UCSZ00)
#define EIGHT_BIT (3<<UCSZ00)
#define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

bool clk = false;
bool pin2 = false;
bool data[] = {1,1,0,0,1,1,0,0};
data_idx = 0;

void controlBit(volatile uint8_t* reg,uint8_t pin,bool val){
        if(val){
                *reg |= _BV(pin);
        }
        else{
                *reg &= ~_BV(pin);
        }
}


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

void USART_Init(){
    // Set Baud Rate
    UBRR0H = BAUD_PRESCALER >> 8;
    UBRR0L = BAUD_PRESCALER;
        
    // Set Frame Format
    UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;
        
    // Enable Receiver and Transmitter
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

void serialWrite(uint8_t DataByte){
    while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
    UDR0 = DataByte;
}

void serialWriteStr(char* DataStr){
        while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
        serialWrite('\n');
        while(*DataStr != '\0'){
                serialWrite(*DataStr++);
        }
        serialWrite('\n');
}

void setTimer(){
    OCR1A = 62499; // delay of 1 second
    TCCR1B |= (1 << CS02);        // prescalar 256
    //TCCR1B |= (1 << CS01);
    //TCCR1B |= (1 << CS00);
    TCCR1B |= (1 << WGM12);   // turns on CTC mode for timer1
    TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
    TCNT1 = 0;                // 16 bit register
}


int main(){
    setIOPins();
    sei();
    USART_Init();
    setTimer();

    while(true){
        //serialWrite('M');
        //_delay_ms(3000);
    }
    return 0;
}

ISR(TIMER1_COMPA_vect){
    
    clk = !clk;
    //controlBit(&PORTB,PORTB5,clk);
    controlBit(&PORTB,PORTB5,data[data_idx++]);
    data_idx%=8;
    controlBit(&PORTB,PORTB4,clk);
    serialWrite('T');
}

ISR(PCINT2_vect){
    serialWrite('P');
    pin2 = !pin2;
    _delay_ms(100);
    char* string = (char*)malloc(40*sizeof(char));
    sprintf(string, "PORTD5 %d",bit_is_set(PIND,PIND5));
    serialWriteStr(string);
    free(string);
    controlBit(&PORTB,PORTB0,bit_is_set(PIND,PIND5));
}
