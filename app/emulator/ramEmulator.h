#ifndef STM32_MCS4_MEMORY_EMULATOR_RAM_EMULATOR_H
#define STM32_MCS4_MEMORY_EMULATOR_RAM_EMULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
  uint8_t mainCharacters[16];
  uint8_t statusCharacters[4];
} RAMRegister;

typedef struct {
  // real 4002 has 4 registers, so in our emulation each bank has 4 chips for 4 registers = 16 registers in total
  RAMRegister registers[16];
  volatile uint8_t *selectedStatusCharacters;
  volatile uint8_t *selectedCharacter;
} RAMBank;

extern RAMBank banks[8];

extern volatile RAMBank *selectedBank;
extern volatile uint8_t *selectedStatusCharacters;
extern volatile uint8_t *selectedCharacter;

void printRAM();
void clearRAM();

#ifdef __cplusplus
}
#endif

#endif // STM32_MCS4_MEMORY_EMULATOR_RAM_EMULATOR_H
