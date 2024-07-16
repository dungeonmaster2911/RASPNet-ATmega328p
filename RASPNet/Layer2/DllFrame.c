/**
* @file DllFrame.c
* @brief This file implements functionalities of the Layer 2
*/

#include "DllFrame.h"
#include "../Utils/UsrUtils.h"
#include "../UART/UARTSetup.h"
#include "../CRC/Custom_CRC32.h"
#include "../Layer3/NetworkFrame.h"
#include "../FrameQueue/Frame_Queue.h"

//#define DLL_DEBUG


#define PREAMBLE 0
#define IDENTIFICATION 8
#define FLAGS 9
#define DEST_ADDR 6
#define SRC_ADDR 7
#define CRC 1
#define SIZE 5
#define PAYLOAD 10


static bool dll_debug = false;
static uint8_t preamble = 0b01111110;

//static uint32_t CRC = 0xFFFFFFFF;

static uint8_t* send_buffer = NULL;

static uint8_t* recv_buffer = NULL;

uint8_t* received_frame = NULL;


static bool preamble_recv = false;


/**
* @brief This function add the frame fields related to Data Link Layer.
* 
* **Further Details**
* - The send_buffer is set to null once the frame is transmitted.
*/
void addDllHeaders(uint8_t* frame,uint8_t* payload, uint8_t payload_len){
	crc32(pCRC(frame),payload,payload_len); /// - Add CRC value to frame
	*pSize(frame) = payload_len; /// - Add Payload length value to frame
	memcpy(pPayload(frame),payload,payload_len); /// - Add Payload to frame	
}

/**
* @brief This function copies a frame to the send_buffer only if send buffer is not NULL.
* 
* **Further Details**
* - The send_buffer is set to null once the frame is transmitted.
*/
bool copyFrameToSendBuffer(uint8_t* dll_frame){
	if(send_buffer == NULL){
		
		//uint8_t frame_size = *(dll_frame+5)+6;
		
		send_buffer = (uint8_t*)malloc(frameSize(dll_frame)*sizeof(uint8_t));
		memcpy(send_buffer,dll_frame,frameSize(dll_frame));
		//printFrame(send_buffer);
		
		//send_buffer = (uint8_t*)malloc(frame_size*sizeof(uint8_t));
		//memcpy(send_buffer,dll_frame,frame_size);
		
		
		//serialWriteStr("frame copied to send_buffer");
		//printFrameToSerial(send_buffer);
		return true;
	}
	else{
		serialWriteStr("send buffer not empty");
		return false;
	}
}


/**
* @brief This function is responsible to transmit each bit of the frame stored in "send_buffer"
* **Further Details**
* - The bits are sent in the order -> MSB to LSB 
*/
void sendDllFrame(){
	
	static uint8_t bytes_to_send = 0;
	static uint8_t bytes_sent = 0;
	static uint8_t bits_sent = 0;
	static bool sending = false;
	//serialWriteStr("11111111");

	if(sending){
		if(bytes_sent < bytes_to_send){
			uint8_t bit = (*(send_buffer+bytes_sent)>>(7-bits_sent)) & 1 ;
			controlBit(&PORTB,PORTB5,bit);
			if(bits_sent == 7){
				if(dll_debug){
					char* string = (char*)malloc(30*sizeof(char));
					sprintf(string, ">> BYTE_%d = %x",bytes_sent,*(send_buffer+bytes_sent));
					serialWriteStr(string);
					free(string);
				}		
				
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
			serialWriteStr(">> FINISHED SENDING\n\n\n");
		}
	}
	else{
		if(send_buffer != NULL){
			//serialWriteStr("444444");
			// new frame to send
			bytes_sent = 0;
			bits_sent = 0;
			//bytes_to_send = *(send_buffer+5)+6;
			bytes_to_send = frameSize(send_buffer);
			sending = true;
			for(int i=0;i<*pSize(send_buffer);i++){
				serialWrite(*(pPayload(send_buffer)+i));
			}
			//serialWrite('\n');
			serialWriteStr(" >> START SENDING");
		}
		else{
			//serialWriteStr("5555555");
			// Nothing to send, force data bit to 0
			controlBit(&PORTB,PORTB5,0);
			//Check if there is a frame to send
			struct Node* node = peekFrameFromQueue();
			if(node!=NULL && node->frame != NULL){
				node = getFrameFromQueue();
				copyFrameToSendBuffer(node->frame);
				free(node->frame);
				free(node);
			}
		}
	}
}



/**
* @brief Check if the received bits match the preample pattern (01111110)
* 
* **Further Details**
*/
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

	if(preamble_buffer==preamble){
		serialWriteStr("<< PREAMBLE MATCH");
		preamble_recv = true; /// - Set Global variable `preamble_recv` if received bits match the preample pattern
		preamble_buffer = 0;
	}
	else{
		preamble_buffer<<=1;
	}
}


/**
* @brief Receive frame bit by bit
* 
* **Further Details**
*/
void recvDllFrame(){
	static uint8_t bytes_recv = 0;
	static uint8_t bits_recv = 0;
	static bool read_payload = false;
	static bool add_data_to_forward_frame = false;
	static struct Node* forward = NULL;

	if(!preamble_recv){
		checkPreambleRecv();
	}
	else{
		if(recv_buffer == NULL){
			recv_buffer = (uint8_t*)malloc(PAYLOAD*sizeof(uint8_t));
			*recv_buffer = preamble;
			bytes_recv++;
		}
		//serialWriteStr("Reading data in this part");
		//preamble_recv = false;
		if(bit_is_set(PIND,PIND5)){
			*(recv_buffer+bytes_recv) |= 1;
		}   
		else{
			*(recv_buffer+bytes_recv) &= ~1;
		}
		if(bits_recv == 7){
			bits_recv=0; // reset bit count to 0 after sending 8 bits

			if(dll_debug){
				char* string = (char*)malloc(30*sizeof(char));
				sprintf(string, "<< BYTE_%d = %x",bytes_recv,*(recv_buffer+bytes_recv));
				serialWriteStr(string);
				free(string);
			}
			bytes_recv++;
			*(recv_buffer+bytes_recv) = 0;
		}
		else{
			*(recv_buffer+bytes_recv)<<=1;
			bits_recv++;
		}
		if(bytes_recv == (SIZE+1) && !read_payload){ /// - Read Payload size realloc sufficient memory for frame
				
			char* string = (char*)malloc(50*sizeof(char));
			//sprintf(string, "CRC_Byte3=%d CRC_Byte2=%d CRC_Byte1=%d CRC_Byte3=%d \nData_Size=%d",*(recv_buffer),*(recv_buffer+1),*(recv_buffer+2),*(recv_buffer+3),*(recv_buffer+4));
			sprintf(string, "<< CRC=%lx  \n<< PAYLOAD_SIZE=%d",*((uint32_t*)pCRC(recv_buffer)),*pSize(recv_buffer));
			serialWriteStr(string);
			free(string);
				
			//free(recv_buffer);
			//bytes_recv = 0;
			//preamble = false;
				
			//Reallocate Memory for recv_buffer based on payload size
			//recv_buffer_size = 5+*(recv_buffer+4);
			void * tmp = realloc(recv_buffer,frameSize(recv_buffer)*sizeof(uint8_t));
			if(tmp !=NULL){
				recv_buffer = tmp;
				read_payload = true;
			}
			else{
				// Reallocation failed, stop reading frame
				//serialWriteStr("Realloc failed !!");
				free(recv_buffer);
				bytes_recv = 0;
				bits_recv = 0;
				preamble_recv = false;
			}
		}
		else if(bytes_recv == (SRC_ADDR+1) && bits_recv == 0){ /// - Check if Frame needs to be forwarded or not
		
			char* string = (char*)malloc(50*sizeof(char));
			sprintf(string, "<< DEST_ADDR = %d  \n<< SRC_ADDR = %d",*pDestAddr(recv_buffer),*pSrcAddr(recv_buffer));
			serialWriteStr(string);
			free(string);
			
			if(checkDestination(recv_buffer)){
				serialWriteStr("<< FORWARD");
				forward = addFrameToQueue(NULL);
				add_data_to_forward_frame = true;
			}
			else{
				serialWriteStr("<< SELF");
			}
		}
		else if(read_payload && bytes_recv == frameSize(recv_buffer)){ /// - Process after reading the complete frame
			serialWriteStr("<< RECV COMPLETE");
			bytes_recv = 0;
			bits_recv= 0;
			read_payload = false;
			preamble_recv = false;
				
				// ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			if(add_data_to_forward_frame){
				uint8_t* forward_frame = (uint8_t*)malloc(frameSize(recv_buffer)*sizeof(uint8_t));
				memcpy(forward_frame,recv_buffer,frameSize(recv_buffer));
				add_data_to_forward_frame = false;
				serialWriteStr("<< ADDED FORWARD FRAME TO QUEUE");
				if(dll_debug){printFrame(forward_frame);}
				//addFrameToQueue(forward_frame);
				forward->frame = forward_frame;
				if(dll_debug){
					printFrame(forward->frame);
				}
				forward = NULL;
			}
			// else{
			
			uint8_t* crc_comp = (uint8_t*)malloc(4*sizeof(uint8_t));						
			crc32(crc_comp,pPayload(recv_buffer),*pSize(recv_buffer));
		
			if(*((uint32_t*)crc_comp) == *((uint32_t*)pCRC(recv_buffer))){
			
				if(received_frame != NULL){
					free(received_frame);
				}
		
				received_frame = (uint8_t*)malloc(frameSize(recv_buffer)*sizeof(uint8_t));
				memcpy(received_frame,recv_buffer,frameSize(recv_buffer));
			}
			else{
				serialWriteStr("CRC Mismatch packet discarded !");
				if(dll_debug){
				printFrame(recv_buffer);
				}
			}
			//}
			free(recv_buffer);
			recv_buffer = NULL;
		}
	}
}


/**
* @brief Get a copy of the received frame
* 
* **Further Details**
*/
uint8_t* getRecvFrame(){
	if(received_frame == NULL){
		return NULL;
	}
	
	
	uint8_t* frame = (uint8_t*)malloc(frameSize(received_frame)*sizeof(uint8_t));
	memcpy(frame,received_frame,frameSize(received_frame)); /// - Make a copy of the `received_frame`
	
	free(received_frame);/// - Clear the `received_frame` buffer
	received_frame = NULL;
	return frame; ///@return pointer to the copy of received frame 
}

void setDllDebug(bool enable){
    dll_debug = enable;
}
