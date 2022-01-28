#ifndef STM32_MCS4_MEMORY_EMULATOR_I4004INTERFACE_H
#define STM32_MCS4_MEMORY_EMULATOR_I4004INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t i4004_readDataBus();
uint8_t i4004_readCMRAM();
uint8_t i4004_readCMROM();
uint8_t i4004_readSync();

#ifdef __cplusplus
}
#endif

#endif //STM32_MCS4_MEMORY_EMULATOR_I4004INTERFACE_H
