#include <stdint.h>

#include "main.h"
#include "externalInterface.h"
#include "i4004Interface.h"
#include "romEmulator.h"

#include "cycleHandler.h"

typedef enum { StageA1, StageA2, StageA3, StageM1, StageM2, StageX1, StageX2, StageX3 } CycleStage;

static volatile uint64_t cycleCount = 0;
static volatile uint8_t running = 0;

static volatile CycleStage currentStage = StageX3;
static volatile CycleStage nextStage = StageA1;

static volatile uint16_t currentAddress = 0;
static volatile uint16_t firstAddressAccessed = 0xFFFF;
static volatile uint8_t currentROMByte = 0;

static volatile uint16_t addressLog[20];
static volatile uint8_t dataBusLog[20];
static volatile uint8_t ptr = 0;

void printCyclerStats() {
  sendExternalMessage("Cycles processed = %08X%08X\r\n", (uint32_t)(cycleCount >> 32U), (uint32_t)(cycleCount & 0xFFFFFFFF));

  for (uint8_t i = 0; i < 19; i++) {
    sendExternalMessage(
      "ROM access %02d: by address %04X returned %02X\r\n",
      i,
      addressLog[i],
      dataBusLog[i]
    );
  }
}

void resetCyclerState() {
  cycleCount = 0;
  currentAddress = 0;
  currentStage = StageX3;
  nextStage = StageA1;
  running = 1;
  firstAddressAccessed = 0xFFFF;
  i4004_freeDataBus();

  ptr = 0;
  for (uint8_t i = 0; i < 10; i++) {
    addressLog[i] = 0;
    dataBusLog[i] = 0;
  }
}

void stopCycler() {
  running = 0;
  i4004_freeDataBus();
}

void handleCyclePhi1Falling() {
  if (!running) {
    return;
  }

  cycleCount++;

  if (i4004_readSync()) {
    // current stage is X3
    currentStage = StageX3;
    nextStage = StageA1;
    return;
  }

  switch (currentStage) {
    case StageA1:
      nextStage = StageA2;
      break;
    case StageA2:
      currentAddress = currentAddress | (uint16_t)(i4004_readDataBus() << 4U);
      nextStage = StageA3;
      break;
    case StageA3:
      currentAddress = currentAddress | (uint16_t)(i4004_readDataBus() << 8U);
      nextStage = StageM1;
      break;
    case StageM1:
      nextStage = StageM2;
      break;
    case StageM2:
      nextStage = StageX1;
      break;
    case StageX1:
      i4004_freeDataBus();
      nextStage = StageX2;
      break;
    case StageX2:
      nextStage = StageX3;
      break;
    default:
      break;
  }
}

void handleCyclePhi2Falling() {
  if (!running) {
    return;
  }

  switch (currentStage) {
    case StageA1:
      currentAddress = i4004_readDataBus();
      break;
    case StageA3:
      currentROMByte = readROM(currentAddress);
      // wait for phi2 rising stage, otherwise i4004 can miss data
      while ((OUT_4004_PHI2_GPIO_Port->IDR & OUT_4004_PHI2_Pin) == 0);
      i4004_writeDataBus(currentROMByte >> 4U);
      break;
    case StageM1:
      // wait for phi2 rising stage, otherwise i4004 can miss data
      while ((OUT_4004_PHI2_GPIO_Port->IDR & OUT_4004_PHI2_Pin) == 0);
      i4004_writeDataBus(currentROMByte & 0xFU);
      break;
    case StageM2:
      if (firstAddressAccessed == 0xFFFF) {
        firstAddressAccessed = currentAddress;
        if (firstAddressAccessed != 0) {
          sendExternalMessage(
            "ERROR! Processor should start from 0x000, but started from 0x%03X",
            firstAddressAccessed
          );
        }
      }
      addressLog[ptr] = currentAddress;
      dataBusLog[ptr] = currentROMByte;
      if (ptr < 19) {
        ptr++;
      }
      break;
    default:
      break;
  }

  currentStage = nextStage;
}