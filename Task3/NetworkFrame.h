#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



uint8_t* addNetworkHeaders(uint8_t dest_addr, uint8_t src_addr, uint8_t* payload, uint8_t* payload_len)  ;

uint8_t checkDestination(uint8_t* frame) ;