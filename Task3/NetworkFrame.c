#include "NetworkFrame.h"

/*
Function to the pack network headers with payload
*/

uint8_t self_addr = 29;

uint8_t* addNetworkHeaders(uint8_t dest_addr, uint8_t src_addr, uint8_t* payload, uint8_t* payload_len) {
    //cout << payload_len + 2 << endl;

    uint8_t* net_packed_payload = (uint8_t*)malloc((*payload_len + 2) * sizeof(uint8_t));  

    *net_packed_payload = dest_addr;

    *(net_packed_payload + 1) = src_addr;

    memcpy(net_packed_payload + 2, payload, *payload_len);

    *payload_len += 2;
    return net_packed_payload;
}

/*
function to check destination of the packet
returns 0=self, 1=another node, 2=broad_cast, -1 = error
*/

uint8_t checkDestination(uint8_t* frame){
	uint8_t ret_val = -1;

	if((frame+5)!=NULL){
		if(*(frame+5) == 0){
			ret_val = 2;
		}
		else if(*(frame+5) == self_addr){
			ret_val = 0;
		}
		else{
			ret_val = 1;
		}
	}
	return ret_val;
}

