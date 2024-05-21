#include "Custom_CRC32.h"
void crc32(uint8_t* crc, uint8_t* payload, uint8_t payload_len) {
    
    *((uint32_t*)crc) = 0x0; // Initial value
    //uint32_t polynomial = 0xEDB88320; // Reversed polynomial for CRC32
    uint32_t polynomial = 0x04C11DB7;

    for (size_t i = 0; i < payload_len; ++i) {
        uint8_t byte = *(payload+i);

        *((uint32_t*)crc) ^= (uint32_t)byte<<24;

        for (int j = 0; j < 8; ++j) { // Process each bit
            if (*((uint32_t*)crc) & 0x80000000) {
                *((uint32_t*)crc) = (*((uint32_t*)crc) << 1) ^ polynomial;
            }
            else {
                *((uint32_t*)crc) = *((uint32_t*)crc) << 1;
            }
        }
    }
}