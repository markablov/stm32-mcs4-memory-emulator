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

static volatile uint8_t srcInstructionIsExecuting = 0;
static volatile uint8_t srcInstructionRegNo = 0;
static volatile uint8_t ioInstructionIsExecuting = 0;

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

// odd numbers (when 1st bit of CM-RAM is set) all maps to bank #0
static uint8_t cmramToBankNumberMap[16] = {
  0, 0, 1, 0, 2, 0, 4, 0,
  3, 0, 5, 0, 6, 0, 7, 0
};

/*
 * Emulate execution of read (read data by CPU from RAM) RAM/IO instructions (done by 4001/4002 in real systems)
 */
static void executeRAMReadInstruction() {
  switch (currentROMByte) {
    // RDM / SBM / ADM
    case 0xE8:
    case 0xE9:
    case 0xEB:
      i4004_writeDataBus(*selectedCharacter);
      break;
    // RD0
    case 0xEC:
      i4004_writeDataBus(selectedStatusCharacters[0]);
      break;
    // RD1
    case 0xED:
      i4004_writeDataBus(selectedStatusCharacters[1]);
      break;
    // RD2
    case 0xEE:
      i4004_writeDataBus(selectedStatusCharacters[2]);
      break;
    // RD3
    case 0xEF:
      i4004_writeDataBus(selectedStatusCharacters[3]);
      break;
    default:
      break;
  }
}

/*
 * Emulate execution of write (write data by CPU to RAM) RAM/IO instructions (done by 4001/4002 in real systems)
 */
static void executeRAMWriteInstruction() {
  uint8_t data = i4004_readDataBus();

  switch (currentROMByte) {
      // WRM
    case 0xE0:
      *selectedCharacter = data;
      break;
      // WR0
    case 0xE4:
      selectedStatusCharacters[0] = data;
      break;
      // WR1
    case 0xE5:
      selectedStatusCharacters[1] = data;
      break;
      // WR2
    case 0xE6:
      selectedStatusCharacters[2] = data;
      break;
      // WR3
    case 0xE7:
      selectedStatusCharacters[3] = data;
      break;
      // WMP
    case 0xE1:
      isrRingBuffer[isrRingBufferWritePtr] = data;
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
    if (srcInstructionIsExecuting) {
      uint8_t charNo = i4004_readDataBus();
      volatile RAMRegister * selectedRegister = &selectedBank->registers[srcInstructionRegNo];
      selectedCharacter = &selectedRegister->mainCharacters[charNo];
      selectedStatusCharacters = selectedRegister->statusCharacters;
      selectedBank->selectedCharacter = selectedCharacter;
      selectedBank->selectedStatusCharacters = selectedStatusCharacters;
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
      if (cmram) {
        uint8_t bankNo = cmramToBankNumberMap[cmram];
        selectedBank = &banks[bankNo];
        selectedStatusCharacters = selectedBank->selectedStatusCharacters;
        selectedCharacter = selectedBank->selectedCharacter;
        ioInstructionIsExecuting = 1;
      } else {
        ioInstructionIsExecuting = 0;
      }
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

      // if there is CMRAM lines active due X2 it means that SRC instruction is executing
      uint8_t cmram = i4004_readCMRAM();
      if (cmram) {
        srcInstructionRegNo = i4004_readDataBus();
        uint8_t bankNo = cmramToBankNumberMap[cmram];
        selectedBank = &banks[bankNo];
        srcInstructionIsExecuting = 1;
        return;
      }

      // there is IO/RAM operation, need to process that by RAM
      if (ioInstructionIsExecuting) {
        executeRAMWriteInstruction();
      }

      srcInstructionIsExecuting = 0;
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
    case StageX1: {
      // there is IO/RAM operation, need to process that by RAM
      if (ioInstructionIsExecuting) {
        executeRAMReadInstruction();
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