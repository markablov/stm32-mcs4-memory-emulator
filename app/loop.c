#include <stdio.h>
#include "main.h"
#include "usbd_cdc_if.h"

#include "loop.h"
#include "pins.h"

void loopTick(void) {
  HAL_GPIO_TogglePin(PIN_PERIPHERAL_LED, PIN_LED_DATA);
  HAL_Delay(200);

  char buffer[200];
  sprintf(buffer, "Clock from variable: %d\r\n", SystemCoreClock);
  CDC_Transmit_FS((uint8_t *)buffer, strlen(buffer) + 1);
  HAL_Delay(3000);
}