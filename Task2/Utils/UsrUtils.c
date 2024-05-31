#include "UsrUtils.h"
#include <stdlib.h>
#include "../UART/UARTSetup.h"
#include "../CRC/Custom_CRC32.h"


void controlBit(volatile uint8_t* reg,uint8_t pin,bool val){
        if(val){
                *reg |= _BV(pin);
        }
        else{
                *reg &= ~_BV(pin);
        }
}
#define PREAMBLE 0
#define IDENTIFICATION 1
#define FLAGS 2
#define DEST_ADDR 3
#define SRC_ADDR 4
#define CRC 5
#define SIZE 9
#define PAYLOAD 10

uint8_t* createFrame(uint8_t* payload, uint8_t payload_len){
        uint8_t* frame = (uint8_t*)malloc((PAYLOAD+payload_len)*sizeof(uint8_t));
        uint8_t* temp;
        /* 
        frame = NULL;
        (frame + SRC_ADDR) = NULL;
        
        (frame + CRC) = NULL;
        (frame + CRC + 1) = NULL; 
        (frame + CRC + 2) = NULL;
        (frame + CRC + 3) = NULL;
        
        *(frame + SIZE) = payload_len;
        
        memcpy(frame+PAYLOAD,payload,payload_len);
        
        
        */
        temp = pPreamble(frame);
        *temp = 0b01111110;
        
        temp = pIdentification(frame);
        *temp = 0;
        
        temp = pFlags(frame);
        *temp = 1;
        
        temp = pDestAddr(frame);
        *temp = 2;
        
        temp = pSrcAddr(frame);
        *temp = 3;
        /*
        temp = pCRC(frame);
        *temp = 1;
        
        temp = pCRC(frame)+1;
        *temp = 1;
        
        temp = pCRC(frame)+2;
        *temp = 1;
        
        temp = pCRC(frame)+3;
        *temp = 1;
        */
        
        crc32(pCRC(frame),payload,payload_len);
        
        *pSize(frame) = payload_len;
        serialWriteStr("test");
        serialWriteStr((char*)payload);
        memcpy(pPayload(frame),payload,payload_len);

        return frame;
}


void printFrame(uint8_t* frame){
        char* string = (char*)malloc(40*sizeof(char));
        
        if(pPreamble(frame)){
                sprintf(string, "PREAMBLE = %d",*pPreamble(frame));
                serialWriteStr(string);
        }
        
        if(pIdentification(frame)){
                sprintf(string, "IDENTIFICATION = %d",*pIdentification(frame));
                serialWriteStr(string);
        }
        
        if(pFlags(frame)){
                sprintf(string, "FLAGS = %d",*pFlags(frame));
                serialWriteStr(string);
        }
        
        if(pDestAddr(frame)){
                sprintf(string, "DEST_ADDR = %d",*pDestAddr(frame));
                serialWriteStr(string);
        }
        
        if(pSrcAddr(frame)){
                sprintf(string, "SRC_ADDR = %d",*pSrcAddr(frame));
                serialWriteStr(string);
        }
        
        if(pCRC(frame)){
                sprintf(string, "CRC = %lx",*((uint32_t*)pCRC(frame)));
                serialWriteStr(string);
        }
        
        if(pSize(frame)){
                sprintf(string, "Size = %d",*pSize(frame));
                serialWriteStr(string);
        }
        
        if(pPayload(frame)){
                for(int i=0;i<*pSize(frame);i++){
                        sprintf(string, "Data_BY_%d  CHAR = %c INT = %d",i,*(pPayload(frame)+i),*(pPayload(frame)+i));
                        serialWriteStr(string);
                }    
        }
        
        free(string);
}

uint8_t* pPreamble(uint8_t* frame){
        return frame + PREAMBLE;
}

uint8_t* pIdentification(uint8_t* frame){
        return frame + IDENTIFICATION;
}

uint8_t* pFlags(uint8_t* frame){
        return frame + FLAGS;
}

uint8_t* pDestAddr(uint8_t* frame){
        return frame + DEST_ADDR;
}

uint8_t* pSrcAddr(uint8_t* frame){
        return frame + SRC_ADDR;
}

uint8_t* pCRC(uint8_t* frame){
        return frame + CRC;
}

uint8_t* pSize(uint8_t* frame){
        return frame + SIZE;
}

uint8_t* pPayload(uint8_t* frame){
        return frame + PAYLOAD;
}

uint8_t frameSize(uint8_t* frame){
	
	if(pSize(frame) != NULL){
		return (PAYLOAD + *(pSize(frame)));
	}
	else{
		return 0;
	}
}
