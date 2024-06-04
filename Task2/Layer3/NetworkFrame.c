#include "NetworkFrame.h"
#include "../Utils/UsrUtils.h"
/*
Function to the pack network headers with payload
*/

uint8_t self_addr = 1;

void addNetworkHeaders(uint8_t dest_addr, uint8_t src_addr, uint8_t* frame) {
    *pDestAddr(frame) = dest_addr;
    *pSrcAddr(frame) = src_addr;
}

/*
function to check destination of the packet
returns 0=broadcast, 1=slef addrd, 2=forward, -1 = error
*/


uint8_t checkDestination(uint8_t* frame){
	uint8_t ret_val = -1;

	if(pDestAddr(frame)!=NULL && pSrcAddr(frame)!=NULL){
		
		if(*(pDestAddr(frame)) == self_addr || *(pSrcAddr(frame)) == self_addr){
			ret_val = 0;
		}
		else{
			ret_val = 1;
		}
	}
	return ret_val;
}

