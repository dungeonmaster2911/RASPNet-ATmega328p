#define F_CPU 12000000UL // Defining the CPU Frequency

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>

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

// variables for program logic

uint8_t time_ct = 0;
uint8_t usr_delay = 10;
bool B5 = true;
bool D4 = true;
uint8_t seq = 0;
bool dir = true;
uint8_t char_data;

// supporting functions for USART

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

void serialWrite(char* DataStr){
        while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
        while(DataStr != '\0'){
                serialWrite(*DataByte++);
        }
        serialWrite("\n");
}

uint8_t USART_ReceivePolling()
{
        uint8_t DataByte;
        //while (( UCSR0A & (1<<RXC0)) == 0) {}; // Do nothing until data have been received
        while (( UCSR0A & (1<<RXC0)) == 0) {};
        DataByte = UDR0 ;
        return DataByte;
}


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

int main() {
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
    sei();
    settimer();
    USART_Init();
    while (1){
        if(( UCSR0A & (1<<RXC0)) != 0){
            char_data = UDR0;
            if(char_data >= 48 && char_data <= 57){
                usr_delay = char_data - '0';
        }
        serialWrite('\n');
        serialWrite('s');
        serialWrite('=');
        serialWrite('0'+ usr_delay);
        serialWrite('\n');

        serialWrite("this is test string")
}

}
return 0;
}


ISR(TIMER1_COMPA_vect){
        time_ct++;

        if(time_ct >= usr_delay*5){
                led_sequence_1();
                time_ct = 0; 
        }
}
