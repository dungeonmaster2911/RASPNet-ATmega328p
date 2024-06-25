#include "PiAtmegaAPI.h"
#include "../UART/UARTSetup.h"
#include "../Layer3/NetworkFrame.h"

void parseCmdFromPi(const char* cmd_str){
	
	int read_int;
	static char data[256];
	static uint8_t dest_addr = 0;
	char* string = (char*)malloc(40*sizeof(char));
	
	static bool payload_ready = false;
	static bool dest_addr_ready = false;
	
	switch(*cmd_str){
		case 'A':
			sscanf(cmd_str,"A%d",&read_int);
			if(read_int > 0 && read_int < 256){
				
				sprintf(string, "Old Self Address: %d",getSelfAddr());
				serialWriteStr(string);
				
				setSelfAddr((uint8_t)read_int);
				
				sprintf(string, "New Self Address: %d",getSelfAddr());
				serialWriteStr(string);
			}
			else{
				serialWriteStr("Address out of range");
			}
			break;
		case 'D':
			memcpy(data,(cmd_str+1),strlen((cmd_str+1)));
			sprintf(string, "Payload: %s\nPayload Length: %d",data,strlen(data));
			serialWriteStr(string);
			payload_ready = true;
			break;
		case 'B':
			sscanf(cmd_str,"B%d",&read_int);
			if(read_int >= 0 && read_int < 256 && read_int != getSelfAddr()){
				dest_addr = read_int;
				sprintf(string, "Destination Address: %d",dest_addr);
				serialWriteStr(string);
				dest_addr_ready = true;
			}
			else{
				serialWriteStr("Address out of range / Destination Addr cannot be same as Src Addr");
			}
			break;
		case 'P':
			if(payload_ready && dest_addr_ready){
				sprintf(string, "Payload: %s\nPayload Length: %d",data,strlen(data));
				serialWriteStr(string);
				sprintf(string, "Destination Address: %d",dest_addr);
				serialWriteStr(string);
				payload_ready = false;
				dest_addr_ready = false;
			}
			else{
				if(!payload_ready){
					serialWriteStr("No payload to send !");
				}
				if(!dest_addr_ready){
					serialWriteStr("Destination address not specified !");
				}
			}
			
			break;
		default:
			serialWriteStr("No case match");
			break;
	}
	
	free(string);
}
