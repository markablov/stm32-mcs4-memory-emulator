#include "main.h"
#include "externalInterface.h"
#include "cycleHandler.h"

#include "commands.h"

void cmdStart(uint8_t *data, uint32_t dataLen) {
  sendExternalMessage("START command has been received, RAM dump size is %d bytes\r\n", dataLen);

  resetCyclerState();
  HAL_GPIO_WritePin(OUT_4004_RESET_GPIO_Port, OUT_4004_RESET_Pin, GPIO_PIN_SET);
}