#include "DllFrame.h"
#include "../Utils/UsrUtils.h"
#include "../UART/UARTSetup.h"
#include "../CRC/Custom_CRC32.h"
#include "../Layer3/NetworkFrame.h"

static uint8_t preamble = 0b01111110;

//static uint32_t CRC = 0xFFFFFFFF;

static uint8_t* send_buffer = NULL;

static uint8_t* recv_buffer = NULL;

uint8_t* received_frame = NULL;


static bool preamble_recv = false;



uint8_t* createDllFrame(uint8_t* payload, uint8_t payload_len){
    // Allocate memory for buffer
    // 1 byte preamble, 4 bytes CRC, 1 byte size, n byte of data length
    
    //uint8_t data_buf_len = strlen(data_buf);
    
    //frame_size = 6+data_buf_len;
    
    uint8_t* dll_frame = (uint8_t*)malloc((6+payload_len)*sizeof(uint8_t));
    
    //Copy preamble to frame
    *dll_frame = preamble;
    
    //Copy CRC to frame
    crc32(dll_frame+1,payload,payload_len);
    /*
    *(dll_frame+1) = CRC>>24; //Bit[31:24]
    *(dll_frame+2) = CRC>>16; //Bit[23:16]
    *(dll_frame+3) = CRC>>8; //Bit[15:8]
    *(dll_frame+4) = CRC; //Bit[7:0]
    */
    
    //Copy data size to frame 
    *(dll_frame+5) = payload_len;
    
    //Copy payload to frame 
    memcpy(dll_frame+6,payload,payload_len);
    printFrameToSerial(dll_frame+1);
    return dll_frame;
}

void printFrameToSerial(uint8_t* dll_frame){
	
	if(dll_frame == NULL){
		serialWriteStr("NULL FRAME !!!");
		return;
	
	}	
	char* string = (char*)malloc(40*sizeof(char));
    
    //sprintf(string, "PREAMBLE=%d",*dll_frame);
    //serialWriteStr(string);
    
    sprintf(string, "CRC=%lx",*((uint32_t*)(dll_frame)));
    serialWriteStr(string);
    /*
    sprintf(string, "CRC_BY2=%d",*(dll_frame+2));
    serialWriteStr(string);
    
    sprintf(string, "CRC_BY1=%d",*(dll_frame+3));
    serialWriteStr(string);
    
    sprintf(string, "CRC_BY0=%d",*(dll_frame+4));
    serialWriteStr(string);
    */
    sprintf(string, "payload_len=%d",*(dll_frame+4));
    serialWriteStr(string);
    
    for(int i=0;i<*(dll_frame+4);i++){
		sprintf(string, "Data_BY_%d  CHAR = %c INT = %d",*(dll_frame+4)-i,*(dll_frame+5+i),*(dll_frame+5+i));
		serialWriteStr(string);
	}    
    free(string);
}

bool copyFrameToSendBuffer(uint8_t* dll_frame){
	if(send_buffer == NULL){
		uint8_t frame_size = *(dll_frame+5)+6;
		send_buffer = (uint8_t*)malloc(frame_size*sizeof(uint8_t));
		memcpy(send_buffer,dll_frame,frame_size);
		//serialWriteStr("frame copied to send_buffer");
		//printFrameToSerial(send_buffer);
		return true;
	}
	else{
		serialWriteStr("send buffer not empty");
		return false;
	}
}


void sendDllFrame(){
	static uint8_t bytes_to_send = 0;
    static uint8_t bytes_sent = 0;
    static uint8_t bits_sent = 0;
    static bool sending = false;
    //serialWriteStr("11111111");
    
    if(sending){
        if(bytes_sent < bytes_to_send){
            //serialWriteStr("22222222");
            uint8_t bit = (*(send_buffer+bytes_sent)>>(7-bits_sent)) & 1 ;
            
            
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
            //serialWriteStr("3333333");
            // All bytes sent
            free(send_buffer);
            send_buffer = NULL;
            sending = false;            
        }
    }
    else{
        if(send_buffer != NULL){
            //serialWriteStr("444444");
            // new frame to send
            bytes_sent = 0;
            bits_sent = 0;
            bytes_to_send = *(send_buffer+5)+6;
            sending = true;
        }
        else{
            //serialWriteStr("5555555");
            // Nothing to send, force data bit to 0
            controlBit(&PORTB,PORTB5,0);
        }
    }
}



void checkPreambleRecv(){
	static uint8_t preamble_buffer = 0;
	
	if(bit_is_set(PIND,PIND5)){
		preamble_buffer |= 1;
	}
    else{
		preamble_buffer &= ~1;
	}
	
	//char* string = (char*)malloc(40*sizeof(char));
	//sprintf(string, "Preamble=%d ",preamble_buffer);
	//serialWriteStr(string);
	//free(string);
        
            
    if(preamble_buffer==0b01111110){
		serialWriteStr("Preamble Match");
		preamble_recv = true;
		preamble_buffer = 0;
		
	}
    else{
		preamble_buffer<<=1;
	}
}

void recvDllFrame(){
	static uint8_t bytes_recv = 0;
	static uint8_t bits_recv = 0;
	static bool read_payload = false;
	static uint8_t recv_buffer_size = 0;
	
	if(!preamble_recv){
		checkPreambleRecv();
	}
	else{
		if(recv_buffer == NULL){
			recv_buffer = (uint8_t*)malloc(5*sizeof(uint8_t));
		}
		
		//serialWriteStr("Reading data in this part");
		//preamble_recv = false;
		if(bit_is_set(PIND,PIND5)){
                *(recv_buffer+bytes_recv) |= 1;
            
            }   
            else{
                *(recv_buffer+bytes_recv) &= ~1;
            }
            
            
            char* string = (char*)malloc(80*sizeof(char));
            sprintf(string, "                              RECV -> bytes_recv= %d  bits_recv=%d  byte_val=%d\nData_Size=%d",bytes_recv,bits_recv,*(recv_buffer+bytes_recv),*(recv_buffer+4)+5);
            serialWriteStr(string);
            free(string);
            
            
            
            if(bits_recv == 7){
                bits_recv=0; // reset bit count to 0 after sending 8 bits
                bytes_recv++;
                *(recv_buffer+bytes_recv) = 0;
            }
            else{
                *(recv_buffer+bytes_recv)<<=1;
                bits_recv++;
            }
            
            
            
            
            if(bytes_recv == 5 && !read_payload){
                serialWriteStr("RECV UNTILL SIZE");
                
                char* string = (char*)malloc(80*sizeof(char));
                //sprintf(string, "CRC_Byte3=%d CRC_Byte2=%d CRC_Byte1=%d CRC_Byte3=%d \nData_Size=%d",*(recv_buffer),*(recv_buffer+1),*(recv_buffer+2),*(recv_buffer+3),*(recv_buffer+4));
                sprintf(string, "#######CRC=%lx  \nData_Size=%d#########",*((uint32_t*)(recv_buffer)),*(recv_buffer+4));
                serialWriteStr(string);
                free(string);
                
                //free(recv_buffer);
                //bytes_recv = 0;
                //preamble = false;
                
                //Reallocate Memory for recv_buffer based on payload size
                recv_buffer_size = 5+*(recv_buffer+4);
                void * tmp = realloc(recv_buffer,recv_buffer_size*sizeof(uint8_t));
                if(tmp !=NULL){
                    recv_buffer = tmp;
                    read_payload = true;
                }
                else{
                    // Reallocation failed, stop reading frame
                    serialWriteStr("Realloc failed !!");
                    free(recv_buffer);
                    bytes_recv = 0;
                    bits_recv = 0;
                    preamble_recv = false;
                }
            }
            else if(bytes_recv >= (*(recv_buffer+4)+5)){
                serialWriteStr("Receive frame completed");
                bytes_recv = 0;
                bits_recv= 0;
                read_payload = false;
                preamble_recv = false;
                
                /*
                uint32_t* crc_comp = (uint32_t*)malloc(sizeof(uint32_t));
                crc32((uint8_t*)crc_comp,recv_buffer+5,*(recv_buffer+4));
				
				char* string = (char*)malloc(40*sizeof(char));
				sprintf(string, "DEST ADDR=%d  SRC ADDR=%d",*(destAddr(recv_buffer)),*(srcAddr(recv_buffer)));
				serialWriteStr(string);
				free(string);
 
                if(*crc_comp == *((uint32_t*)recv_buffer)){
					for(int i=0;i<*(recv_buffer+4);i++){
						char* string = (char*)malloc(40*sizeof(char));
						sprintf(string, "PAYLOAD_BYTE_%d = %c",i,*(recv_buffer+5+i));
						serialWriteStr(string);
						free(string);
						//serialWrite(*(recv_buffer+5+i));
					}
					serialWrite('\n');
				}
				else{
					serialWriteStr("CRC Mismatch Discard frame !!");
				}
				
                free(crc_comp);   
                */
                
                printFrameToSerial(recv_buffer);
                
                if(received_frame != NULL){
					free(received_frame);
					received_frame = NULL;
				}
				
				
				received_frame = (uint8_t*)malloc(frameSize(recv_buffer)*sizeof(uint8_t));
                memcpy(received_frame,recv_buffer,frameSize(recv_buffer));
                printFrameToSerial(received_frame);
                
                
                free(recv_buffer);
                recv_buffer = NULL;
            }
	}

}


uint8_t frameSize(uint8_t* frame){
	
	if((frame+4) != NULL){
		return 5+*(frame+4);
	}
	else{
		return 0;
	}
}



uint8_t* getRecvFrame(){
	return received_frame;
}

