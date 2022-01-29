#ifndef STM32_MCS4_MEMORY_EMULATOR_COMMANDS_H
#define STM32_MCS4_MEMORY_EMULATOR_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void cmdStart(uint8_t *data, uint32_t dataLen);
void cmdStop();

#ifdef __cplusplus
}
#endif

#endif //STM32_MCS4_MEMORY_EMULATOR_COMMANDS_H
