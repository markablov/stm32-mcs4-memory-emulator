#include "externalInterface.h"

#include "ramEmulator.h"

RAMBank banks[8];

volatile RAMBank *selectedBank = &banks[0];
volatile uint8_t *selectedStatusCharacters = banks[0].registers[0].statusCharacters;
volatile uint8_t *selectedCharacter = &(banks[0].registers[0].mainCharacters[0]);

void printRAM() {
  for (uint8_t bankNo = 0; bankNo < 8; bankNo++) {
    RAMBank *bank = &banks[bankNo];
    sendExternalMessage("Bank #%d\r\n", bankNo);
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
    bank->selectedStatusCharacters = bank->registers[0].statusCharacters;
    bank->selectedCharacter = &(bank->registers[0].mainCharacters[0]);

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
