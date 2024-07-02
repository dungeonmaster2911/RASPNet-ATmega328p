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


void USART_Init()
{
        // Set Baud Rate
        UBRR0H = BAUD_PRESCALER >> 8;
        UBRR0L = BAUD_PRESCALER;
        
        // Set Frame Format
        UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;
        
        // Enable Receiver and Transmitter
        UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

void serialWrite(uint8_t DataByte)
{
        while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
        UDR0 = DataByte;
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
        sei();
        USART_Init();
        setTimer();

        while(true){
           serialWrite('M');
           _delay_ms(3000);
        }
        return 0;
}

ISR(TIMER1_COMPA_vect){
	serialWrite('I');
}
