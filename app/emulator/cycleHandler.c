#include <stdint.h>

#include "externalInterface.h"
#include "i4004Interface.h"

#include "cycleHandler.h"

typedef enum { StageA1, StageA2, StageA3, StageM1, StageM2, StageX1, StageX2, StageX3 } CycleStage;

static uint64_t cycleCount = 0;

CycleStage nextStage = StageX3;

void printCyclerStats() {
  sendExternalMessage("Cycles processed = %08X%08X\r\n", (uint32_t)(cycleCount >> 32U), (uint32_t)(cycleCount & 0xFFFFFFFF));
}

void resetCyclerState() {
  cycleCount = 0;
}

void handleCycle() {
  cycleCount++;

  if (i4004_readSync()) {
    // current stage is X3
    nextStage = StageA1;
    return;
  }

  CycleStage currentStage = nextStage;

  // previous stage
  switch (currentStage) {
    case StageA1:
      nextStage = StageA2;
      break;
    case StageA2:
      nextStage = StageA3;
      break;
    case StageA3:
      nextStage = StageM1;
      break;
    case StageM1:
      nextStage = StageM2;
      break;
    case StageM2:
      nextStage = StageX1;
      break;
    case StageX1:
      nextStage = StageX2;
      break;
    case StageX2:
      nextStage = StageX3;
      break;
    default:
      break;
  }
}