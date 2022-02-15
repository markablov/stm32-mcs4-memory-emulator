#ifndef STM32_MCS4_MEMORY_EMULATOR_CYCLE_HANDLER_H
#define STM32_MCS4_MEMORY_EMULATOR_CYCLE_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

void handleCyclePhi1Falling();
void handleCyclePhi2Falling();
void printCyclerStats();
void runCycler();
void stopCycler();

#ifdef __cplusplus
}
#endif

#endif // STM32_MCS4_MEMORY_EMULATOR_CYCLE_HANDLER_H
