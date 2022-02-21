#include "externalInterface.h"
#include "cycleHandler.h"
#include "romEmulator.h"
#include "ramEmulator.h"

#include "commands.h"

void cmdStart(uint8_t *data, uint32_t dataLen) {
  sendExternalMessage("START command has been received, RAM dump size is %d bytes\r\nBINARY_START", dataLen);
  initROM(data, dataLen);
  clearRAM();
  runCycler();
}