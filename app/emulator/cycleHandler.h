#ifndef STM32_MCS4_MEMORY_EMULATOR_CYCLE_HANDLER_H
#define STM32_MCS4_MEMORY_EMULATOR_CYCLE_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

void handleCycle();
void printCyclerStats();
void resetCyclerState();

#ifdef __cplusplus
}
#endif

#endif // STM32_MCS4_MEMORY_EMULATOR_CYCLE_HANDLER_H
