#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "main.h"
#include "usart.h"

#include "externalInterface.h"

#define MAX_MSG_LEN 100

void initExternalInterface() {
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

void processInputSymbol(uint8_t symbol) {
  sendExternalMessage("You have sent: %c\r\n", symbol);
}

void sendExternalMessage(const char *format, ...) {
  va_list args;
  va_start(args, format);

  char buf[MAX_MSG_LEN];

  vsnprintf(buf, sizeof(buf), format, args);
  HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 10);

  va_end(args);
}
