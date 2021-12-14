#include <stdio.h>
#include <string.h>
#include "main.h"
#include "usart.h"

#include "externalInterface.h"

void processInputSymbol(uint8_t symbol) {
  char buf[100];
  sprintf(buf, "You have sent: %c\r\n", symbol);
  HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 10);
}
