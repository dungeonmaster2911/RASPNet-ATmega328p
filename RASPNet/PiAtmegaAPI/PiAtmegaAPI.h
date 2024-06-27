#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>




void parseCmdFromPi(const char* cmd_str);
void sendTestMsg(char c);
