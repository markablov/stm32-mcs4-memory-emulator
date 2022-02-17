#include "ramEmulator.h"

typedef struct {
  uint8_t mainCharacters[16];
  uint8_t statusCharacters[4];
} RAMRegister;

typedef struct {
  // real 4002 has 4 registers, so in our emulation each bank has 4 chips for 4 registers = 16 registers in total
  RAMRegister registers[16];
  uint8_t selectedRegister;
  uint8_t selectedCharacter;
} RAMBank;

static RAMBank banks[8];

void RAM_selectRegister(uint8_t bankNo, uint8_t regNo) {
  banks[bankNo].selectedRegister = regNo;
}

void RAM_selectCharacter(uint8_t bankNo, uint8_t charNo) {
  banks[bankNo].selectedCharacter = charNo;
}

uint8_t RAM_readStatusCharacter(uint8_t bankNo, uint8_t statusCharNo) {
  RAMBank * bank = &banks[bankNo];
  return bank->registers[bank->selectedRegister].statusCharacters[statusCharNo];
}

void RAM_writeStatusCharacter(uint8_t bankNo, uint8_t statusCharNo, uint8_t value) {
  RAMBank * bank = &banks[bankNo];
  bank->registers[bank->selectedRegister].statusCharacters[statusCharNo] = value;
}