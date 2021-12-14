#include "main.h"
#include "usart.h"

#include "loop.h"

void loopTick(void) {
//  uint8_t msg[] = "Hello World !!!\r\n";
//  HAL_UART_Transmit(&huart1, msg, sizeof(msg),10);

  HAL_GPIO_TogglePin(OUT_LED_DATA_GPIO_Port, OUT_LED_DATA_Pin);
  HAL_Delay(1000);
}