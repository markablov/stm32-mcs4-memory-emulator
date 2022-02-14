#include <stdlib.h>
#include <string.h>

#include "romEmulator.h"

static uint8_t *rom;
static uint32_t romSize;

void initROM(uint8_t *data, uint32_t dataLen) {
  if (rom) {
    free(rom);
  }

  romSize = dataLen;
  rom = malloc((size_t)dataLen);
  memcpy(rom, data, (size_t)dataLen);
}

uint8_t readROM(uint16_t address) {
  if (address >= romSize) {
    return 0x00;
  }

  return rom[address];
}
