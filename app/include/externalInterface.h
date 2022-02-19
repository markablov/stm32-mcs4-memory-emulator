#ifndef STM32_MCS4_MEMORY_EMULATOR_EXTERNALINTERFACE_H
#define STM32_MCS4_MEMORY_EMULATOR_EXTERNALINTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void processInputSymbol(uint8_t symbol);
void initExternalInterface();
void sendExternalMessage(const char *format, ...);
void sendExternalBytes(uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif //STM32_MCS4_MEMORY_EMULATOR_EXTERNALINTERFACE_H
