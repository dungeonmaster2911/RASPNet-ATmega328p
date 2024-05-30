#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint8_t* createDllFrame(uint8_t* payload, uint8_t payload_len);

void printFrameToSerial(uint8_t* dll_frame);

bool copyFrameToSendBuffer(uint8_t* dll_frame);

void sendDllFrame();

void checkPreambleRecv();

void recvDllFrame();

uint8_t* getRecvFrame();

uint8_t frameSize(uint8_t* frame);
