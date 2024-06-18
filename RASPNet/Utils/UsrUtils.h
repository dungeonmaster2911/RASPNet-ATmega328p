#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void controlBit(volatile uint8_t* reg,uint8_t pin,bool val);

uint8_t* createFrame(uint8_t* payload, uint8_t payload_len);

void printFrame(uint8_t* frame);



uint8_t* pIdentification(uint8_t* frame);

uint8_t* pFlags(uint8_t* frame);

uint8_t* pPreamble(uint8_t* frame);

uint8_t* pDestAddr(uint8_t* frame);

uint8_t* pSrcAddr(uint8_t* frame);

uint8_t* pCRC(uint8_t* frame);

uint8_t* pSize(uint8_t* frame);

uint8_t* pPayload(uint8_t* frame);

uint8_t frameSize(uint8_t* frame);
