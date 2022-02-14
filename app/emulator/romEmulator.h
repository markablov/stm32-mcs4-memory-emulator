#ifndef STM32_MCS4_MEMORY_EMULATOR_ROM_EMULATOR_H
#define STM32_MCS4_MEMORY_EMULATOR_ROM_EMULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void initROM(uint8_t *data, uint32_t dataLen);
uint8_t readROM(uint16_t address);

#ifdef __cplusplus
}
#endif

#endif // STM32_MCS4_MEMORY_EMULATOR_ROM_EMULATOR_H
