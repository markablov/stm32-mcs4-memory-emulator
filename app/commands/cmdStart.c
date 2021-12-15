#include "externalInterface.h"

#include "commands.h"

void cmdStart(uint8_t *data, uint32_t dataLen) {
  sendExternalMessage("START command has been received, RAM dump size is %d bytes\r\n", dataLen);
}