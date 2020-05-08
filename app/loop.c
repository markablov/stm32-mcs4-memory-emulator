#include "main.h"
#include "usbd_cdc_if.h"

#include "loop.h"

void loopTick(void) {
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_12);
  HAL_Delay(200);
  uint8_t buffer[] = "Hello, World!\r\n";
  CDC_Transmit_FS(buffer, sizeof(buffer));
  HAL_Delay(1000);
}