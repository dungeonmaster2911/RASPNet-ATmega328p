#include "PiAtmegaAPI.h"
#include "../UART/UARTSetup.h"
#include "../Utils/UsrUtils.h"
#include "../Layer2/DllFrame.h"
#include "../Layer3/NetworkFrame.h"
#include "../FrameQueue/Frame_Queue.h"

void parseCmdFromPi(const char* cmd_str){
	
	int read_int;
	static char  data[256];
	static uint8_t dest_addr = 0;
	char* string = (char*)malloc(40*sizeof(char));
	
	static bool payload_ready = false;
	static bool dest_addr_ready = false;
	
	switch(*cmd_str){
		case 'A':
			sscanf(cmd_str,"A %d",&read_int);
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
			memset(data,0,strlen(data));
			memcpy(data,(cmd_str+2),strlen((cmd_str+1)));
			sprintf(string, "Payload: %s\nPayload Length: %d",data,strlen(data));
			serialWriteStr(string);
			payload_ready = true;
			break;
		case 'B':
			sscanf(cmd_str,"B %d",&read_int);
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
				
				uint8_t*frame = createFrame((uint8_t*)data,strlen(data));
				addNetworkHeaders(dest_addr,getSelfAddr(),frame);
				addFrameToQueue(frame);
				
				memset(data,0,strlen(data));
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
		case 'X':
			/// Test cases !!!
			sscanf(cmd_str,"X %d",&read_int);
			sendTestMsg((uint8_t)read_int);
			break;
		case 'V':
			sscanf(cmd_str,"V %d",&read_int);
			if(read_int == 1){
				setDllDebug(true);
			}
			else{
				setDllDebug(false);
			}
			break;
		default:
			serialWriteStr("No case match");
			serialWriteStr("HELP \
							\nSelf address value (1-255) --> A xx\
							\nPayload message (max length 255 characters) --> D xyz\
							\nDestination address value (0-255) --> B xx\
							\nSend the message --> P");
			break;
	}
	
	free(string);
}

void sendTestMsg(uint8_t c){
	
	static char* pd = "casex";
	uint8_t* frame = NULL;
	
	switch(c){
		case 1:
			pd = "case1";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(2,1,frame);
			addFrameToQueue(frame);
			break;
		case 2:
		    pd = "case2";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(1,2,frame);
			addFrameToQueue(frame);
			break;
		case 3:
			pd = "case3";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(0,1,frame);
			addFrameToQueue(frame);
			break;
		case 4:
			pd = "case4";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(5,6,frame);
			addFrameToQueue(frame);
			break;
		case 5:
			pd = "case5";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(0,6,frame);
			addFrameToQueue(frame);
			break;
		case 6:
			// clear queue
			serialWriteStr("CLEAR QUEUE !!!!");
			createFrameQueue();
			return;
			break;
		default:
			return;
			break;
	}
}
