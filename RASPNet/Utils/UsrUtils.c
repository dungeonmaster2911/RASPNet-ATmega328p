/**
* @file UsrUtils.c
* @brief This file implements some custom utility functions for the RASPNet project.
*/

#include "UsrUtils.h"
#include <stdlib.h>
#include "../UART/UARTSetup.h"
#include "../CRC/Custom_CRC32.h"

#define PREAMBLE 0
#define IDENTIFICATION 1
#define FLAGS 2
#define DEST_ADDR 3
#define SRC_ADDR 4
#define CRC 5
#define SIZE 9
#define PAYLOAD 10

/**
* @brief Set/Unset specific bits of the provided register
* 
* @param volatile uint8_t* reg reference to PORT/REGISTER
* @param uint8_t pin bit value of the pin of a specific port
* @param bool val True = Set OR False = Unset
*/
void controlBit(volatile uint8_t* reg,uint8_t pin,bool val){
        if(val){
                *reg |= _BV(pin);
        }
        else{
                *reg &= ~_BV(pin);
        }
}


/**
* @brief Creates a complete frame.
* 
* **Further Details**
*/
uint8_t* createFrame(uint8_t* payload, uint8_t payload_len){
        uint8_t* frame = (uint8_t*)malloc((PAYLOAD+payload_len)*sizeof(uint8_t));
        uint8_t* temp;
       
        temp = pPreamble(frame);
        *temp = 0b01111110; /// - Add Preamble value to frame
        
        temp = pIdentification(frame);
        *temp = 0; /// - Add Identifaction value to frame
        
        temp = pFlags(frame);
        *temp = 1; /// - Add flag value to frame
        
        temp = pDestAddr(frame);
        *temp = 2;
        
        temp = pSrcAddr(frame);
        *temp = 3;
        
        
        crc32(pCRC(frame),payload,payload_len); /// - Add CRC value to frame
        
        *pSize(frame) = payload_len; /// - Add Payload length value to frame
        serialWriteStr("test");
        serialWriteStr((char*)payload);
        memcpy(pPayload(frame),payload,payload_len); /// - Add Payload to frame

        return frame;
}

/**
* @brief Prints all headers fields of the frame to UART channel
* 
* @param uint8_t* frame pointer to the loaded frame
*/
void printFrame(uint8_t* frame){
        char* string = (char*)malloc(40*sizeof(char));
        
        if(pPreamble(frame)){
                sprintf(string, "PREAMBLE = %d",*pPreamble(frame));
                serialWriteStr(string);
        }
        else{
                serialWriteStr("NULL FRAME");
                return;
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

/**
* @brief Returns a pointer to the PREAMBLE field of the frame
* 
* @param uint8_t* frame pointer to the loaded frame
*/

uint8_t* pPreamble(uint8_t* frame){
        return frame + PREAMBLE;
}

/**
* @brief Returns a pointer to the IDENTIFICATION field of the frame
* 
* @param uint8_t* frame pointer to the loaded frame
*/
uint8_t* pIdentification(uint8_t* frame){
        return frame + IDENTIFICATION;
}

/**
* @brief Returns a pointer to the FLAGS field of the frame
* 
* @param uint8_t* frame pointer to the loaded frame
*/
uint8_t* pFlags(uint8_t* frame){
        return frame + FLAGS;
}

/**
* @brief Returns a pointer to the DEST_ADDR field of the frame
* 
* @param uint8_t* frame pointer to the loaded frame
*/
uint8_t* pDestAddr(uint8_t* frame){
        return frame + DEST_ADDR;
}

/**
* @brief Returns a pointer to the SRC_ADDR field of the frame
* 
* @param uint8_t* frame pointer to the loaded frame
*/
uint8_t* pSrcAddr(uint8_t* frame){
        return frame + SRC_ADDR;
}

/**
* @brief Returns a pointer to the CRC field of the frame
* 
* @param uint8_t* frame pointer to the loaded frame
*/
uint8_t* pCRC(uint8_t* frame){
        return frame + CRC;
}

/**
* @brief Returns a pointer to the SIZE field of the frame
* 
* @param uint8_t* frame pointer to the loaded frame
*/
uint8_t* pSize(uint8_t* frame){
        return frame + SIZE;
}

/**
* @brief Returns a pointer to the PAYLOAD field of the frame
* 
* @param uint8_t* frame pointer to the loaded frame
*/
uint8_t* pPayload(uint8_t* frame){
        return frame + PAYLOAD;
}

/**
* @brief Returns size of the complete frame in bytes.
* 
* @param uint8_t* frame pointer to the loaded frame
* 
* **Further Details**
* - frame_size = Size of all headers (fixed) + size of payload (variable)
*   
*/
uint8_t frameSize(uint8_t* frame){
	
	if(pSize(frame) != NULL){
		return (PAYLOAD + *(pSize(frame))); ///@return uint8_t value of frame size in bytes
	}
	else{
		return 0;
	}
}
