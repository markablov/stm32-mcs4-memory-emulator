#include "externalInterface.h"
#include "i4004Interface.h"

#include "commands.h"

void cmdStart(uint8_t *data, uint32_t dataLen) {
  sendExternalMessage("START command has been received, RAM dump size is %d bytes\r\n", dataLen);

  sendExternalMessage(
    "Value of registers: data = %X, CMRAM = %X, CMROM = %d, SYNC = %d\r\n",
    i4004_readDataBus(),
    i4004_readCMRAM(),
    i4004_readCMROM(),
    i4004_readSync()
  );
}