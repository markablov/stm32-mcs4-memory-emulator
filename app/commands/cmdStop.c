#include "externalInterface.h"
#include "cycleHandler.h"

#include "commands.h"

void cmdStop() {
  sendExternalMessage("STOP command has been received\r\n");
  sendExternalMessage("Cycles processed = %d\r\n", getCycleCount());
}