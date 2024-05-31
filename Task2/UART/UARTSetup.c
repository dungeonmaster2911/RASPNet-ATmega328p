#include "UARTSetup.h"

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
        //serialWrite('\n');
        while(*DataStr != '\0'){
                serialWrite(*DataStr++);
        }
        serialWrite('\n');
}

uint8_t serialRead(bool block)
{
        uint8_t DataByte = '\0';
        //while (( UCSR0A & (1<<RXC0)) == 0) {}; // Do nothing until data have been received
        if(block){
                while (( UCSR0A & (1<<RXC0)) == 0) {};
                DataByte = UDR0 ;
        }
        else{
                if(( UCSR0A & (1<<RXC0)) != 0){
                     DataByte = UDR0 ;     
             }
     }
     return DataByte;
}

char* serialReadStr(){
    char* string = (char*)malloc(40*sizeof(char));
    
    char char_data = serialRead(true);
    int idx = 0;
    
    while(char_data != 13){
        *(string + idx) = char_data;
        char_data = serialRead(true);
        idx++;
    }
    
    *(string + idx) = '\0';
    //serialWriteStr(string);
    return string; 
}
