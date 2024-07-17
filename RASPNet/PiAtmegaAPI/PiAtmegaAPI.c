#include "PiAtmegaAPI.h"
#include "../UART/UARTSetup.h"
#include "../Utils/UsrUtils.h"
#include "../Layer2/DllFrame.h"
#include "../Layer3/NetworkFrame.h"
#include "../FrameQueue/Frame_Queue.h"
#include "../Layer1/ISR_Clock_Data.h"

/**
* @file PiAtmegaAPI.c
* @brief This file implements the API for communication between RPi and Atmega via UART.
*/


/**
* @brief Function to check if any input is available from RPi
*/
void checkCmdFromPi(){
	char* serial_str = serialReadStr();
	if(serial_str){
		parseCmdFromPi(serial_str);
		free(serial_str);
	}
}


/**
* @brief Function to parse the command received from RPi and execute further actions
*/
void parseCmdFromPi(const char* cmd_str){
	
	int read_int;
	static char  data[256];
	static uint8_t dest_addr = 0;
	char* string = (char*)malloc(40*sizeof(char));
	
	static bool payload_ready = false;
	static bool dest_addr_ready = false;
	
	switch(*cmd_str){
		case 'A':
			/// - Change device address
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
			/// - Set Payload data
			memset(data,0,strlen(data));
			memcpy(data,(cmd_str+2),strlen((cmd_str+1)));
			sprintf(string, "Payload: %s\nPayload Length: %d",data,strlen(data));
			serialWriteStr(string);
			payload_ready = true;
			break;
		case 'B':
			/// - Set Destination address for payload
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
			/// - Send the message 
			if(payload_ready && dest_addr_ready){
				sprintf(string, "Payload: %s\nPayload Length: %d",data,strlen(data));
				serialWriteStr(string);
				sprintf(string, "Destination Address: %d",dest_addr);
				serialWriteStr(string);
				
				uint8_t*frame = createFrame((uint8_t*)data,strlen(data));
				addNetworkHeaders(dest_addr,getSelfAddr(),frame);
				addDllHeaders((uint8_t*)data,strlen(data),frame);
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
			/// - Send Test messages 
			sscanf(cmd_str,"X %d",&read_int);
			sendTestMsg((uint8_t)read_int);
			break;
		case 'V':
			/// - Enable/Disable verbose logs 
			sscanf(cmd_str,"V %d",&read_int);
			if(read_int == 1){
				setDllDebug(true);
				serialWriteStr("Verbose logs enabled");
			}
			else{
				setDllDebug(false);
				serialWriteStr("Verbose logs disabled");
			}
			break;
		case 'I':
			/// - Enable/Disable RASPNet communication 
			sscanf(cmd_str,"I %d",&read_int);
			if(read_int == 1){
				setCommEnable(true);
				serialWriteStr("Communication enabled");
			}
			else{
				setCommEnable(false);
				serialWriteStr("Communication disabled");
			}
			break;
			
		default:
			/// - Print Help information in case of invalid command 
			serialWriteStr("No case match");
			serialWriteStr("HELP \
							\nSelf address value (1-255) --> A xx\
							\nPayload message (max length 255 characters) --> D xyz\
							\nDestination address value (0-255) --> B xx\
							\nSend the message --> P\
							\nSend Test messages (1-6)--> X xx\
							\nEnable/Disable Verbose logs -->V 1 / V 0\
							\nEnable/Disable Communication -->I 1 / I 0");
			break;
	}
	
	free(string);
}


/**
* @brief Function to send various test messages
*/
void sendTestMsg(uint8_t c){
	
	static char* pd = "casex";
	uint8_t* frame = NULL;
	
	switch(c){
		case 1:
			pd = "case1";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(2,1,frame);
			addDllHeaders((uint8_t*)pd,strlen(pd),frame);
			addFrameToQueue(frame);
			break;
		case 2:
		    pd = "case2";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(1,2,frame);
			addDllHeaders((uint8_t*)pd,strlen(pd),frame);
			addFrameToQueue(frame);
			break;
		case 3:
			pd = "case3";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(0,1,frame);
			addDllHeaders((uint8_t*)pd,strlen(pd),frame);
			addFrameToQueue(frame);
			break;
		case 4:
			pd = "case4";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(5,6,frame);
			addDllHeaders((uint8_t*)pd,strlen(pd),frame);
			addFrameToQueue(frame);
			break;
		case 5:
			pd = "case5";
			frame = createFrame((uint8_t*)pd,strlen(pd));
			addNetworkHeaders(0,6,frame);
			addDllHeaders((uint8_t*)pd,strlen(pd),frame);
			addFrameToQueue(frame);
			break;
		case 6:
			//Case 6 -> Clear the frame queue
			serialWriteStr("CLEAR QUEUE !!!!");
			createFrameQueue();
			return;
			break;
		default:
			return;
			break;
	}
}
