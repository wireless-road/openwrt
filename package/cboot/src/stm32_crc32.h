
#ifndef _STM32_CRC32_H_
#define _STM32_CRC32_H_

#include <stdint.h>

#define POLY_USED_IN_STM32		0x04C11DB7
#define CRC_INITIALVALUE		0xFFFFFFFF

uint32_t stm32_sw_crc32_by_bit(uint32_t crc32, uint8_t pBuffer[], uint32_t NumOfByte);
uint32_t stm32_sw_crc32_by_byte(uint32_t crc32, uint8_t pBuffer[], uint32_t NumOfByte);
uint32_t stm32_sw_crc32_by_nibble(uint32_t crc32, uint8_t pBuffer[], uint32_t NumOfByte);

#endif // _STM32_CRC32_H_



