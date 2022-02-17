#include "externalInterface.h"
#include "cycleHandler.h"

#include "commands.h"

void cmdStop() {
  stopCycler();
  sendExternalMessage("\xFF\xFF\xFF\xFFSTOP command has been received\r\n");
  printCyclerStats();
}