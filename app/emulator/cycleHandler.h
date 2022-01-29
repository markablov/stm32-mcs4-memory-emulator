#ifndef STM32_MCS4_MEMORY_EMULATOR_CYCLE_HANDLER_H
#define STM32_MCS4_MEMORY_EMULATOR_CYCLE_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void handleCycle();
uint64_t getCycleCount();

#ifdef __cplusplus
}
#endif

#endif // STM32_MCS4_MEMORY_EMULATOR_CYCLE_HANDLER_H
