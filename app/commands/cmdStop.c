#include "main.h"
#include "externalInterface.h"
#include "cycleHandler.h"

#include "commands.h"

void cmdStop() {
  HAL_GPIO_WritePin(OUT_4004_RESET_GPIO_Port, OUT_4004_RESET_Pin, GPIO_PIN_RESET);
  stopCycler();

  sendExternalMessage("STOP command has been received\r\n");
  printCyclerStats();
}