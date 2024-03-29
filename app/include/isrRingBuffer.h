#ifndef STM32_MCS4_MEMORY_EMULATOR_ISR_RING_BUFFER_H
#define STM32_MCS4_MEMORY_EMULATOR_ISR_RING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ISR_RING_BUFFER_LENGTH 1024

extern uint8_t isrRingBuffer[ISR_RING_BUFFER_LENGTH];

// could be updated in ISR
extern volatile uint16_t isrRingBufferWritePtr;
extern volatile uint16_t isrRingBufferReadPtr;

#ifdef __cplusplus
}
#endif

#endif //STM32_MCS4_MEMORY_EMULATOR_ISR_RING_BUFFER_H
