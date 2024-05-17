#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

void controlBit(volatile uint8_t* reg,uint8_t pin,bool val);
