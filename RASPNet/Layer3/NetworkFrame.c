/**
* @file NetworkFrame.c
* @brief This file implements functionalities Layer 3.
*/


#include "NetworkFrame.h"
#include "../Utils/UsrUtils.h"


static uint8_t self_addr = 1;


/**
* @brief Function to the pack network headers in the frame
* @param uint8_t dest_addr value of Destinatiion address
* @param uint8_t src_addr value of Source address
* @param uint8_t* frame pointer to the frame
* 
* **Further Details**
*/
void addNetworkHeaders(uint8_t dest_addr, uint8_t src_addr, uint8_t* frame) {
    *pDestAddr(frame) = dest_addr; /// - Add Destination Address value in the frame
    *pSrcAddr(frame) = src_addr; /// - Add Source Address value in the frame
}

/**
* @brief Checks if the current frame needs to be forwarded or not
*
* @param uint8_t* frame pointer to the frame
*/
uint8_t checkDestination(uint8_t* frame){
	uint8_t ret_val = -1;

	if(pDestAddr(frame)!=NULL && pSrcAddr(frame)!=NULL){
		/// If Destination address or Source address is same as self address return 0
		if(*(pDestAddr(frame)) == self_addr || *(pSrcAddr(frame)) == self_addr){
			ret_val = 0;
		}
		else{
			ret_val = 1;
		}
	}
	return ret_val; ///@return 0 = No need to forward the frame OR 1 = Need to forward the frame
}


uint8_t getSelfAddr(){
	return self_addr;
}

void setSelfAddr(uint8_t addr){
	self_addr = addr;
}


uint8_t* processNetworkFrame(uint8_t* frame){
	if(*pSrcAddr(frame)  == getSelfAddr() ){
		free(frame); /// Discard frame after reaching source
		frame = NULL;
	}
	else if((*pDestAddr(frame) == getSelfAddr()) || (*pDestAddr(frame) == 0)){
		printFrame(frame);
	}
	return frame;
}
