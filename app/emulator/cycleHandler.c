#include <stdint.h>

#include "main.h"
#include "externalInterface.h"
#include "i4004Interface.h"
#include "romEmulator.h"
#include "ramEmulator.h"

#include "cycleHandler.h"

typedef enum { StageA1, StageA2, StageA3, StageM1, StageM2, StageX1, StageX2, StageX3 } CycleStage;
typedef enum { StateStale, StateResetting, StateRunning } CyclerState;

static volatile uint64_t instructionCount = 0;
static volatile CyclerState state = StateStale;

static volatile CycleStage currentStage = StageX3;
static volatile CycleStage nextStage = StageA1;

static volatile uint16_t currentAddress = 0;
static volatile uint8_t currentROMByte = 0;

// better to send RAM outputs in some idle stage
static volatile uint8_t delayedRAMOutputByte = 0;
static volatile uint8_t delayedRAMOutputBytePresence = 0;

// RAM/ROM bank, that would be used for SRC instruction
static volatile uint8_t ramBankToSetAccessAddress = 0xFF;
// RAM/ROM bank, that would be used for RAM/IO instruction
static volatile uint8_t ramBankToExecuteInstruction = 0xFF;

void printCyclerStats() {
  sendExternalMessage("Instruction cycles processed = %08X%08X\r\n", (uint32_t)(instructionCount >> 32U), (uint32_t)(instructionCount & 0xFFFFFFFF));
}

/*
 * Just marks cycler that we want to run it (need to set RESET signal at determined stage)
 */
void runCycler() {
  state = StateResetting;
}

void stopCycler() {
  HAL_GPIO_WritePin(OUT_4004_RESET_GPIO_Port, OUT_4004_RESET_Pin, GPIO_PIN_RESET);
  state = StateStale;
}

static void startCycler() {
  HAL_GPIO_WritePin(OUT_4004_RESET_GPIO_Port, OUT_4004_RESET_Pin, GPIO_PIN_SET);
  instructionCount = 0;
  state = StateRunning;
}

static void executeRAMInstruction() {
  switch (currentROMByte) {
    // RD0
    case 0xEC:
      i4004_writeDataBus(RAM_readStatusCharacter(ramBankToExecuteInstruction, 0));
      break;
    // WR0
    case 0xE4:
      RAM_writeStatusCharacter(ramBankToExecuteInstruction, 0, i4004_readDataBus());
      break;
      // WMP
    case 0xE1:
      delayedRAMOutputBytePresence = 1;
      delayedRAMOutputByte = i4004_readDataBus();
      break;

    default:
      break;
  }
}

void handleCyclePhi1Falling() {
  if (i4004_readSync()) {
    instructionCount++;
    // current stage is X3
    currentStage = StageX3;
    if (ramBankToSetAccessAddress != 0xFF) {
      RAM_selectCharacter(ramBankToSetAccessAddress, i4004_readDataBus());
    }

    nextStage = StageA1;
    return;
  }

  switch (currentStage) {
    case StageA1:
      currentAddress = i4004_readDataBus();
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
    case StageM2: {
      // if there is CMRAM lines active due M2 it means that IO/RAM instruction is executing
      uint8_t cmram = i4004_readCMRAM();
      ramBankToExecuteInstruction = cmram == 0 ? 0xFF : 0;
      nextStage = StageX1;
      break;
    }
    case StageX1:
      i4004_freeDataBus();
      nextStage = StageX2;
      break;
    case StageX2: {
      nextStage = StageX3;
      // turn on CPU on predictable stage
      if (state == StateResetting) {
        startCycler();
        return;
      }

      // there is IO/RAM operation, need to process that by RAM
      if (ramBankToExecuteInstruction != 0xFF) {
        executeRAMInstruction();
      }

      break;
    }
    default:
      break;
  }
}

void handleCyclePhi2Falling() {
  if (state != StateRunning) {
    currentStage = nextStage;
    return;
  }

  switch (currentStage) {
    case StageA1:
      if (delayedRAMOutputBytePresence) {
        delayedRAMOutputBytePresence = 0;
        sendByteFast(delayedRAMOutputByte);
      }
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
    case StageX2: {
      // if there is CMRAM lines active due X2 it means that SRC instruction is executing
      uint8_t cmram = i4004_readCMRAM();
      if (cmram != 0) {
        ramBankToSetAccessAddress = 0;
        RAM_selectRegister(ramBankToSetAccessAddress, i4004_readDataBus());
      } else {
        ramBankToSetAccessAddress = 0xFF;
      }

      break;
    }
    case StageX3:
      // wait for phi2 rising stage, otherwise i4004 can miss data
      while ((OUT_4004_PHI2_GPIO_Port->IDR & OUT_4004_PHI2_Pin) == 0);
      i4004_freeDataBus();
      break;
    default:
      break;
  }

  currentStage = nextStage;
}