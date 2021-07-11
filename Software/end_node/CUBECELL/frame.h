#pragma once

#include <stdio.h>
#include <stdint.h>

uint16_t crctable[2] ={ 0x0000, 0x1189};

uint16_t CalculateCRC16( // Call example CalculateCRC16("<09M", 4);
    const void *c_ptr, // Pointer to byte array to perform CRC on
    size_t len)        // Number of bytes to CRC
{
    uint16_t crc = 0xFFFF; // Seed for CRC calculation
    const uint8_t *c = (const uint8_t *)c_ptr;

    while (len--)
        crc = (crc << 8) ^ crctable[((crc >> 8) ^ *c++)];

    return crc;
}

