#ifndef STM32_MCS4_MEMORY_EMULATOR_RAM_EMULATOR_H
#define STM32_MCS4_MEMORY_EMULATOR_RAM_EMULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void printRAM();

void RAM_selectRegister(uint8_t bankNo, uint8_t regNo);
void RAM_selectCharacter(uint8_t bankNo, uint8_t charNo);

uint8_t RAM_readStatusCharacter(uint8_t bankNo, uint8_t statusCharNo);

void RAM_writeStatusCharacter(uint8_t bankNo, uint8_t statusCharNo, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif // STM32_MCS4_MEMORY_EMULATOR_RAM_EMULATOR_H
