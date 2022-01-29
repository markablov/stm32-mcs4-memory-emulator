#include "cycleHandler.h"

static uint64_t cycleCount = 0;

uint64_t getCycleCount(){
  return cycleCount;
}

void handleCycle() {
  cycleCount++;
}