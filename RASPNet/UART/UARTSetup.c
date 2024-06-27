/**
* @file UARTSetup.c
* @brief This file implements configuration and basic functionalities for UART communication.
*/

#include "UARTSetup.h"


/**
* @brief Configure UART Communication
* 
* **Further Details**
*/
void USART_Init(){
    /// - Set Baud Rate to 9600
    UBRR0H = BAUD_PRESCALER >> 8;
    UBRR0L = BAUD_PRESCALER;
        
    /// - Set Frame Format
    UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;
        
    /// - Enable Receiver and Transmitter
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

/**
* @brief Writes 1 byte data to UART Tx Channel
* 
* @param uint8_t DataByte value of byte data
*/
void serialWrite(uint8_t DataByte){
    while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
    UDR0 = DataByte;
}

/**
* @brief Writes a null terminated string to UART Tx channel
* @param char* DataStr pointer to a null terminated string
*/
void serialWriteStr(char* DataStr){
        while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
        while(*DataStr != '\0'){
                serialWrite(*DataStr++);
        }
        serialWrite('\n');
}

/**
* @brief Reads 1 byte of data from UART Rx channel
* 
* @param bool block
* 
* **Further Details**
*/
uint8_t serialRead(bool block)
{
        uint8_t DataByte = '\0';
        /// - If block is true wait for data to be available on UART Rx channel
        if(block){
                while (( UCSR0A & (1<<RXC0)) == 0) {};
                DataByte = UDR0 ;
        }
        /// - If block is false read data from UART Rx channel, but do not wait.
        else{
                if(( UCSR0A & (1<<RXC0)) != 0){
                     DataByte = UDR0 ;     
             }
     }
     return DataByte;///@return uint8_t byte value of read data
}

/**
* @brief Reads char data from UART Rx channel, until enter key is pressed
* 
*/
char* serialReadStr(){
    
    if(( UCSR0A & (1<<RXC0)) == 0){
        return NULL;
    }
    
    char* string = (char*)malloc(40*sizeof(char));
    char char_data = serialRead(true);
    int idx = 0;
    
    while(char_data != 13){
        *(string + idx) = char_data;
        char_data = serialRead(true);
        idx++;
    }
    
    *(string + idx) = '\0';
    return string; ///@return char* pointer to read string
}
