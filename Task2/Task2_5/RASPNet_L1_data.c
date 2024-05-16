#define F_CPU 12000000UL // Defining the CPU Frequency

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
bool data[] = {1,0,1,0,1,0,1,0};
uint8_t data_idx = 0;
uint8_t preamble = 0b01111110;
uint32_t CRC = 0xFFFFFFFF;
char* data_buf = "AAD";

uint8_t* dll_frame;
bool dll_frame_created = false;
uint8_t frame_size = 0;
uint8_t* send_frame;
bool sending = false;



uint8_t recv_preamble;
//uint8_t recv_crc[4];
//uint8_t recv_size;
uint8_t* recv_frame;
uint8_t recv_frame_size;

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

void setTimer(){
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


void createFrame(){
    // Allocate memory for buffer
    // 1 byte preamble, 4 bytes CRC, 1 byte size, n byte of data length
    uint8_t data_buf_len = strlen(data_buf);
    
    frame_size = 6+data_buf_len;
    
    dll_frame = (uint8_t*)malloc(frame_size*sizeof(uint8_t));
    
    //Copy preamble to frame
    *dll_frame = preamble;
    //Copy CRC to frame
    *(dll_frame+1) = CRC>>24; //Bit[31:24]
    *(dll_frame+2) = CRC>>16; //Bit[23:16]
    *(dll_frame+3) = CRC>>8; //Bit[15:8]
    *(dll_frame+4) = CRC; //Bit[7:0]
    //Copy data size to frame 
    *(dll_frame+5) = data_buf_len;
    memcpy(dll_frame+6,data_buf,data_buf_len);
    dll_frame_created = true;
    serialWriteStr("Frame Created");
    char* string = (char*)malloc(40*sizeof(char));
    sprintf(string, "Data Buff Len=%d  Frame size=%d ",data_buf_len,frame_size);
    serialWriteStr(string);
    free(string);
}

void sendFrame(){
    static uint8_t bytes_sent = 0;
    static uint8_t bits_sent = 0;
    static uint8_t* send_frame = NULL ;
    //serialWriteStr("11111111");
    if(sending && send_frame!= NULL){
        if(bytes_sent < frame_size){
            //serialWriteStr("22222222");
            uint8_t bit = (*(send_frame+bytes_sent)>>(7-bits_sent)) & 1 ;
            
            
            char* string = (char*)malloc(40*sizeof(char));
            sprintf(string, "Bytes Sent=%d   Bits sent=%d Data=%d",bytes_sent,bits_sent,bit);
            serialWriteStr(string);
            free(string);
            
            controlBit(&PORTB,PORTB5,bit);
            if(bits_sent == 7){
                bits_sent=0; // reset bit count to 0 after sending 8 bits
                bytes_sent++;
            }
            else{
                bits_sent++;
            }
            
        }
        else{
            serialWriteStr("3333333");
            // All bytes sent
            free(send_frame);
            sending = false;            
        }
    }
    else{
        if(dll_frame_created){
            serialWriteStr("444444");
            // new frame to send
            send_frame = (uint8_t*)malloc(frame_size*sizeof(uint8_t));
            memcpy(send_frame,dll_frame,frame_size);
            free(dll_frame);
            dll_frame_created = false;
            bytes_sent = 0;
            bits_sent = 0;
            sending = true;
        }
        else{
            //serialWriteStr("5555555");
            // Nothing to send, force data bit to 0
            controlBit(&PORTB,PORTB5,0);
        }
    }
}

void receiveFrame(){
    static bool preamble = false;
    static bool recv_data_buff = false;
    static uint8_t bits_recv=0;
    static uint8_t bytes_recv=0; 
    
    if(!preamble){
        if(bit_is_set(PIND,PIND5)){
            recv_preamble |= 1;
        }
        else{
            recv_preamble &= ~1;
        }
        
        char* string = (char*)malloc(40*sizeof(char));
        sprintf(string, "Preamble=%d ",recv_preamble);
        serialWriteStr(string);
        free(string);
        
        
        
        if(recv_preamble==0b01111110){
            serialWriteStr("Preamble Match");
            recv_frame= (uint8_t*)malloc(5*sizeof(uint8_t));
            *recv_frame = 0;
            recv_preamble = 0;
            bits_recv=0;
            preamble = true;
            bytes_recv=0; 
        }
        else{
            recv_preamble<<=1;
        }
        
        
    }
    else{
        //if(!recv_data_buff){
        if(true){
            if(bit_is_set(PIND,PIND5)){
                *(recv_frame+bytes_recv) |= 1;
            
            }   
            else{
                *(recv_frame+bytes_recv) &= ~1;
            }
            
            
            char* string = (char*)malloc(40*sizeof(char));
            sprintf(string, "bytes_recv= %d  byte_val=%d",bytes_recv,*(recv_frame+bytes_recv));
            serialWriteStr(string);
            free(string);
            
            
            
            if(bits_recv == 7){
                bits_recv=0; // reset bit count to 0 after sending 8 bits
                bytes_recv++;
                *(recv_frame+bytes_recv) = 0;
            }
            else{
                *(recv_frame+bytes_recv)<<=1;
                bits_recv++;
            }
            if(bytes_recv == 5 && !recv_data_buff){
                serialWriteStr("RECV UNTILL SIZE");
                
                char* string = (char*)malloc(40*sizeof(char));
                sprintf(string, "CRC_Byte3=%d CRC_Byte2=%d CRC_Byte1=%d CRC_Byte3=%d \nData_Size=%d",*(recv_frame),*(recv_frame+1),*(recv_frame+2),*(recv_frame+3),*(recv_frame+4));
                serialWriteStr(string);
                free(string);
                
                //free(recv_frame);
                //bytes_recv = 0;
                //preamble = false;
                
                //Reallocate Memory for recv_frame based on payload size
                recv_frame_size = 5+*(recv_frame+4);
                void * tmp = realloc(recv_frame,recv_frame_size*sizeof(uint8_t));
                if(tmp !=NULL){
                    recv_frame = tmp;
                    recv_data_buff = true;
                }
                else{
                    // Reallocation failed, stop reading frame
                    serialWriteStr("Realloc failed !!");
                    free(recv_frame);
                    preamble = false;
                }
            }
            else if(bytes_recv == (*(recv_frame+4)+5)){
                serialWriteStr("Receive frame completed");
                for(int i=0;i<*(recv_frame+4);i++){
                    serialWrite(*(recv_frame+5+i));
                } 
                free(recv_frame);
                preamble = false;
            }
        }
    }    
}


int main(){
    setIOPins();
    sei();
    USART_Init();
    setTimer();

    while(true){
        //serialWrite('M');
        //_delay_ms(3000);
        uint8_t char_data = serialRead(false);
        if(char_data == 's'){
            serialWriteStr("Will create frame...");
            createFrame();
        }
    }
    return 0;
}

ISR(TIMER1_COMPA_vect){
    
    clk = !clk;
    //controlBit(&PORTB,PORTB5,clk);
    controlBit(&PORTB,PORTB4,clk);
    //controlBit(&PORTB,PORTB5,data[data_idx++]);
    //data_idx%=8;
    sendFrame();
    //serialWrite('T');
}

ISR(PCINT2_vect){
    //serialWrite('P');
    controlBit(&PORTB,PORTB0,bit_is_set(PIND,PIND5));
    receiveFrame();
}
