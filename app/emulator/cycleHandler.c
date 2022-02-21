#include <stdint.h>

#include "main.h"
#include "externalInterface.h"
#include "i4004Interface.h"
#include "romEmulator.h"
#include "ramEmulator.h"
#include "isrRingBuffer.h"

#include "cycleHandler.h"

typedef enum { StageA1, StageA2, StageA3, StageM1, StageM2, StageX1, StageX2, StageX3 } CycleStage;
typedef enum { StateStale, StateResetting, StateRunning } CyclerState;

static volatile uint64_t instructionCount = 0;
static volatile CyclerState state = StateStale;

static volatile CycleStage currentStage = StageX3;
static volatile CycleStage nextStage = StageA1;

static volatile uint16_t currentAddress = 0;
static volatile uint8_t currentROMByte = 0;

// RAM/ROM bank, that would be used for SRC instruction
static volatile uint8_t ramBankToSetAccessAddress = 0xFF;
// RAM/ROM bank, that would be used for RAM/IO instruction
static volatile uint8_t ramBankToExecuteInstruction = 0xFF;

/*
 * Print some statistics and logs about current run of i4004
 */
void printCyclerStats() {
  sendExternalMessage("Instruction cycles processed = %08X%08X\r\n", (uint32_t)(instructionCount >> 32U), (uint32_t)(instructionCount & 0xFFFFFFFF));
}

/*
 * Just marks cycler that we want to run it (need to set RESET signal at determined stage)
 */
void runCycler() {
  state = StateResetting;
}

/*
 * Stop i4004 (via RESET pin)
 */
void stopCycler() {
  HAL_GPIO_WritePin(OUT_4004_RESET_GPIO_Port, OUT_4004_RESET_Pin, GPIO_PIN_RESET);
  state = StateStale;
}

/*
 * Actually run i4004 (by setting RESET pin)
 */
static void startCycler() {
  HAL_GPIO_WritePin(OUT_4004_RESET_GPIO_Port, OUT_4004_RESET_Pin, GPIO_PIN_SET);
  instructionCount = 0;
  state = StateRunning;
}

/*
 * Convert CMRAM lines into RAM bank number
 */
static uint8_t getRAMBankNoFromCMRAM(uint8_t cmram) {
  switch (cmram) {
    case 0b0000:
      return 0xFF;
    case 0b0010:
      // CM_RAM1 => bank #1
      return 1;
    case 0b0100:
      // CM_RAM2 => bank #2
      return 2;
    case 0b1000:
      // CM_RAM3 => bank #3
      return 3;
    case 0b0110:
      // CM_RAM1 + CM_RAM2 => bank #4
      return 4;
    case 0b1010:
      // CM_RAM1 + CM_RAM3 => bank #5
      return 5;
    case 0b1100:
      // CM_RAM2 + CM_RAM3 => bank #6
      return 6;
    case 0b1110:
      // CM_RAM1 + CM_RAM2 + CM_RAM3 => bank #7
      return 7;
    default:
      // if CM_RAM0 set, it always mean bank #0
      return 0;
  }
}

/*
 * Emulate execution of RAM/IO instructions (done by 4001/4002 in real systems)
 */
static void executeRAMInstruction() {
  switch (currentROMByte) {
    // RDM / SBM / ADM
    case 0xE8:
    case 0xE9:
    case 0xEB:
      i4004_writeDataBus(RAM_readMainCharacter(ramBankToExecuteInstruction));
      break;
    // RD0
    case 0xEC:
      i4004_writeDataBus(RAM_readStatusCharacter(ramBankToExecuteInstruction, 0));
      break;
    // RD1
    case 0xED:
      i4004_writeDataBus(RAM_readStatusCharacter(ramBankToExecuteInstruction, 1));
      break;
    // RD2
    case 0xEE:
      i4004_writeDataBus(RAM_readStatusCharacter(ramBankToExecuteInstruction, 2));
      break;
    // RD3
    case 0xEF:
      i4004_writeDataBus(RAM_readStatusCharacter(ramBankToExecuteInstruction, 3));
      break;
    // WRM
    case 0xE0:
      RAM_writeMainCharacter(ramBankToExecuteInstruction, i4004_readDataBus());
      break;
    // WR0
    case 0xE4:
      RAM_writeStatusCharacter(ramBankToExecuteInstruction, 0, i4004_readDataBus());
      break;
    // WR1
    case 0xE5:
      RAM_writeStatusCharacter(ramBankToExecuteInstruction, 1, i4004_readDataBus());
      break;
    // WR2
    case 0xE6:
      RAM_writeStatusCharacter(ramBankToExecuteInstruction, 2, i4004_readDataBus());
      break;
    // WR3
    case 0xE7:
      RAM_writeStatusCharacter(ramBankToExecuteInstruction, 3, i4004_readDataBus());
      break;
    // WMP
    case 0xE1:
      isrRingBuffer[isrRingBufferWritePtr] = i4004_readDataBus();
      isrRingBufferWritePtr = (isrRingBufferWritePtr + 1) % ISR_RING_BUFFER_LENGTH;
      break;

    default:
      break;
  }
}

/*
 * First phase when we want to proceed some actions (read data pins / emulate instruction execution / ...)
 */
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
      ramBankToExecuteInstruction = getRAMBankNoFromCMRAM(i4004_readCMRAM());
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

/*
 * Second phase when we want to proceed some actions (output ROM content / select RAM registers / ...)
 */
void handleCyclePhi2Falling() {
  if (state != StateRunning) {
    currentStage = nextStage;
    return;
  }

  switch (currentStage) {
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
      ramBankToSetAccessAddress = getRAMBankNoFromCMRAM(i4004_readCMRAM());
      if (ramBankToSetAccessAddress != 0xFF) {
        RAM_selectRegister(ramBankToSetAccessAddress, i4004_readDataBus());
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