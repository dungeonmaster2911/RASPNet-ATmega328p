#include "NetworkFrame.h"
#include "../Utils/UsrUtils.h"


uint8_t self_addr = 1;


/**
* - Function to the pack network headers in the frame
*/
void addNetworkHeaders(uint8_t dest_addr, uint8_t src_addr, uint8_t* frame) {
    *pDestAddr(frame) = dest_addr; /// Add Destination Address value in the frame
    *pSrcAddr(frame) = src_addr; /// Add Source Address value in the frame
}

/*
* - Checks if the current frame needs to be forwarded or not 
*/
uint8_t checkDestination(uint8_t* frame){
	uint8_t ret_val = -1;

	if(pDestAddr(frame)!=NULL && pSrcAddr(frame)!=NULL){
		/// If Destination address or Source address is same as self address return 0
		/// 0 = No need to forward the frame
		if(*(pDestAddr(frame)) == self_addr || *(pSrcAddr(frame)) == self_addr){
			ret_val = 0;
		}
		/// 1 = Need to forward the frame
		else{
			ret_val = 1;
		}
	}
	return ret_val;
}

