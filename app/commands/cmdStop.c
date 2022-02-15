#include "externalInterface.h"
#include "cycleHandler.h"

#include "commands.h"

void cmdStop() {
  stopCycler();
  sendExternalMessage("STOP command has been received\r\n");
  printCyclerStats();
}