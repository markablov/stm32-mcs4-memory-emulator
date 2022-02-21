#include "externalInterface.h"

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

void printRAM() {
  for (uint8_t bankNo = 0; bankNo < 8; bankNo++) {
    RAMBank *bank = &banks[bankNo];
    sendExternalMessage("Bank #%d, selected register is %d, selected character is %d\r\n", bankNo, bank->selectedRegister, bank->selectedCharacter);
    for (uint8_t regNo = 0; regNo < 16; regNo++) {
      RAMRegister *reg = &bank->registers[regNo];
      uint8_t *mainChars = reg->mainCharacters;
      uint8_t *statusChars = reg->statusCharacters;

      sendExternalMessage(
        "  Reg #%02d, main [%X, %X, %X, %X, %X, %X, %X, %X, %X, %X, %X, %X, %X, %X, %X, %X], status [%X, %X, %X, %X]\r\n",
        regNo,
        mainChars[0], mainChars[1], mainChars[2], mainChars[3], mainChars[4], mainChars[5], mainChars[6], mainChars[7],
        mainChars[8], mainChars[9], mainChars[10], mainChars[11], mainChars[12], mainChars[13], mainChars[14], mainChars[15],
        statusChars[0], statusChars[1], statusChars[2], statusChars[3]
      );
    }
  }
}

void clearRAM() {
  for (uint8_t bankNo = 0; bankNo < 8; bankNo++) {
    RAMBank *bank = &banks[bankNo];
    bank->selectedRegister = 0;
    bank->selectedCharacter = 0;

    for (uint8_t regNo = 0; regNo < 16; regNo++) {
      RAMRegister *reg = &bank->registers[regNo];

      uint8_t *mainChars = reg->mainCharacters;
      for (uint8_t charNo = 0; charNo < 16; charNo++) {
        mainChars[charNo] = 0;
      }

      uint8_t *statusChars = reg->statusCharacters;
      statusChars[0] = 0;
      statusChars[1] = 0;
      statusChars[2] = 0;
      statusChars[3] = 0;
    }
  }
}

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

uint8_t RAM_readMainCharacter(uint8_t bankNo) {
  RAMBank * bank = &banks[bankNo];
  return bank->registers[bank->selectedRegister].mainCharacters[bank->selectedCharacter];
}

void RAM_writeStatusCharacter(uint8_t bankNo, uint8_t statusCharNo, uint8_t value) {
  RAMBank * bank = &banks[bankNo];
  bank->registers[bank->selectedRegister].statusCharacters[statusCharNo] = value;
}

void RAM_writeMainCharacter(uint8_t bankNo, uint8_t value) {
  RAMBank * bank = &banks[bankNo];
  bank->registers[bank->selectedRegister].mainCharacters[bank->selectedCharacter] = value;
}