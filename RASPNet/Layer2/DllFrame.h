#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void addDllHeaders(uint8_t* payload, uint8_t payload_len, uint8_t* frame);

bool copyFrameToSendBuffer(uint8_t* dll_frame);

void sendDllFrame();

void checkPreambleRecv();

void recvDllFrame();

uint8_t* getRecvFrame();

void setDllDebug(bool enable);
