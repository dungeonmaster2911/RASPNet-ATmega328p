#include "PiAtmegaAPI.h"
#include "../UART/UARTSetup.h"

void parseCmdFromPi(const char* cmd_str){
	
	int read_int;
	
	switch(*cmd_str){
		case 'A':
			sscanf(cmd_str,"A%d",&read_int);
			if(read_int > 0 && read_int < 256){
				char* string = (char*)malloc(40*sizeof(char));
				sprintf(string, "Address: %d",read_int);
				serialWriteStr(string);
				free(string);
			}
			else{
				serialWriteStr("Out of range");
			}
			break;
		default:
			serialWriteStr("No case match");
			break;
	}
}
