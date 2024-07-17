#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



void checkCmdFromPi();
void parseCmdFromPi(const char* cmd_str);
void sendTestMsg(uint8_t c);
