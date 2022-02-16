#include "externalInterface.h"
#include "cycleHandler.h"

#include "commands.h"

void cmdStop() {
  stopCycler();
  sendExternalMessage("\r\nSTOP command has been received\r\n");
  printCyclerStats();
}