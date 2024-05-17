#include "UsrUtils.h"
#include <stdlib.h>

void controlBit(volatile uint8_t* reg,uint8_t pin,bool val){
        if(val){
                *reg |= _BV(pin);
        }
        else{
                *reg &= ~_BV(pin);
        }
}
