#ifndef STM32_MCS4_MEMORY_EMULATOR_I4004INTERFACE_H
#define STM32_MCS4_MEMORY_EMULATOR_I4004INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

volatile uint8_t i4004_readDataBus();
void i4004_writeDataBus(uint8_t data);
void i4004_freeDataBus();

volatile uint8_t i4004_readCMRAM();
volatile uint8_t i4004_readCMROM();
volatile uint8_t i4004_readSync();

#ifdef __cplusplus
}
#endif

#endif //STM32_MCS4_MEMORY_EMULATOR_I4004INTERFACE_H
