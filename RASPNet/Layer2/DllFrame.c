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

/// Position for Preamble field in frame 
#define PREAMBLE 0
/// Position for Identification field in frame
#define IDENTIFICATION 8
/// Position for Flag field in frame
#define FLAGS 9
/// Position for Destination address field in frame
#define DEST_ADDR 6
/// Position for Source address field in frame
#define SRC_ADDR 7
/// Position for CRC field in frame
#define CRC 1
/// Position for Size field in frame
#define SIZE 5
/// Position for Payloaad field in frame
#define PAYLOAD 10


/// @brief  If true, program will give verbose prints for send and receive
static bool dll_debug = false;

/// @brief Stores the Preamble value
static uint8_t preamble = 0b01111110;

/// @brief Intended frame to be transmitted is loaded to send_buffer
static uint8_t* send_buffer = NULL;

/// @brief When receiving is ongoing data is stored in recv_buffer
static uint8_t* recv_buffer = NULL;

/// @brief After receiving is completed, full frame is copied to received_frame, so the program can start to receive the next frame if available
static uint8_t* received_frame = NULL;

/// @brief Set to if preamble is received
static bool preamble_recv = false;


/**
* @brief This function add the frame fields related to Data Link Layer.
* 
* **Further Details**
*/
void addDllHeaders(uint8_t* payload, uint8_t payload_len, uint8_t* frame){
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
		send_buffer = (uint8_t*)malloc(frameSize(dll_frame)*sizeof(uint8_t));
		memcpy(send_buffer,dll_frame,frameSize(dll_frame));
		//printFrame(send_buffer);
		return true;
	}
	else{
		serialWriteStr("ERROR: send buffer not empty");
		return false;
	}
}

/**
* @brief This function is responsible to transmit each bit of the frame 
* stored in "send_buffer"
* **Further Details**
*/
void sendDllFrame(){
	
	static uint8_t bytes_to_send = 0;
	static uint8_t bytes_sent = 0;
	static uint8_t bits_sent = 0;
	static bool sending = false;
	/// If currently sending a frame
	if(sending){
		if(bytes_sent < bytes_to_send){ 
			/// - For each byte send send bit MSB -> LSB
			uint8_t bit = (*(send_buffer+bytes_sent)>>(7-bits_sent))&1; /// - Rerieve each bit from current byte
			controlBit(&PORTB,PORTB5,bit); /// - Reflect bit value on Data Send pin
			if(bits_sent == 7){ // Check if all bits (8) of the byte are sent
				// Debug print: Raw hex value for each sent byte
				if(dll_debug){	
					char* string = (char*)malloc(30*sizeof(char));
					sprintf(string, ">> BYTE_%d = %x",
							bytes_sent,
							*(send_buffer+bytes_sent));
					serialWriteStr(string);
					free(string);
				}		
				bits_sent=0; // Reset Bit count to 0
				bytes_sent++;
			}
			else{
				bits_sent++;
			} 
		}
		else{
			/// When frame is sent completely, empty send_buffer
			free(send_buffer);
			send_buffer = NULL;
			sending = false;
			serialWriteStr(">> FINISHED SENDING\n\n\n");
		}
	}
	/// If currently not sending a frame
	else{
		if(send_buffer != NULL){
			/// - Check if send buffer is loaded 
			bytes_sent = 0;
			bits_sent = 0;
			bytes_to_send = frameSize(send_buffer);
			sending = true;
			
			/* Notify user by printning the paylod data being sent when 
			 * starting to send
			 */
			for(int i=0;i<*pSize(send_buffer);i++){
				serialWrite(*(pPayload(send_buffer)+i));
			}
			serialWriteStr(" >> START SENDING");
		}
		else{
			/// - If Nothing to send, force data send bit to 0
			controlBit(&PORTB,PORTB5,0);
			/// - Check if there is a frame to send in the frameQueue
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

	if(preamble_buffer==preamble){
		serialWriteStr("<< PREAMBLE MATCH");
		preamble_recv = true; /// - Set Global variable `preamble_recv`if received bits match the preample pattern
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
	/// If already received preamble
	else{
		/// - Check if recv buffer is null, allocate memory and add preamble
		if(recv_buffer == NULL){
			recv_buffer = (uint8_t*)malloc(PAYLOAD*sizeof(uint8_t));
			*recv_buffer = preamble;
			bytes_recv++;
		}
		
		///  - Read bit value on Data IN pin and store to current byte
		if(bit_is_set(PIND,PIND5)){
			*(recv_buffer+bytes_recv) |= 1;
		}   
		else{
			*(recv_buffer+bytes_recv) &= ~1;
		}
		
		// Check if all bits of a byte are received
		if(bits_recv == 7){
			bits_recv=0; // Reset recv bit count to 0 
			
			// Debug print: Raw hex value for each received byte
			if(dll_debug){
				char* string = (char*)malloc(30*sizeof(char));
				sprintf(string, "<< BYTE_%d = %x",bytes_recv,*(recv_buffer+bytes_recv));
				serialWriteStr(string);
				free(string);
			}
			bytes_recv++;
			//Clear the next byte value to avoid garbage
			*(recv_buffer+bytes_recv) = 0;
		}
		else{
			*(recv_buffer+bytes_recv)<<=1;
			bits_recv++;
		}
		
		/// Check for SIZE field of the frame
		if(bytes_recv == (SIZE+1) && !read_payload){ /// - Read Payload size realloc sufficient memory for frame
				
			char* string = (char*)malloc(50*sizeof(char));

			sprintf(string, "<< CRC=%lx  \n<< PAYLOAD_SIZE=%d",*((uint32_t*)pCRC(recv_buffer)),*pSize(recv_buffer));
			serialWriteStr(string);
			free(string);

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
		/// Check for Destination and Source Address field of the frame
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
		/// Check for reading the complete frame
		else if(read_payload && bytes_recv == frameSize(recv_buffer)){
			serialWriteStr("<< RECV COMPLETE");
			bytes_recv = 0;
			bits_recv= 0;
			read_payload = false;
			preamble_recv = false;

			/// - If frame need to be forwared add data to forward frame node
			if(add_data_to_forward_frame){
				uint8_t* forward_frame = (uint8_t*)malloc(frameSize(recv_buffer)*sizeof(uint8_t));
				memcpy(forward_frame,recv_buffer,frameSize(recv_buffer));
				add_data_to_forward_frame = false;
				
				serialWriteStr("<< ADDED FORWARD FRAME TO QUEUE");
				if(dll_debug){printFrame(forward_frame);}
				
				forward->frame = forward_frame;
				forward = NULL;
			}

			/// - Validate CRC
			uint8_t* crc_comp = (uint8_t*)malloc(4*sizeof(uint8_t));						
			crc32(crc_comp,pPayload(recv_buffer),*pSize(recv_buffer));
			/// - If CRC matches copy frame from receive buffer to received frame, else discar frame
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
/**
 * @brief Function set/unset `dll_debug`
 */
void setDllDebug(bool enable){
    dll_debug = enable;
}
