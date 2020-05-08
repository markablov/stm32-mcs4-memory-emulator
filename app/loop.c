#include "main.h"
#include "usbd_cdc_if.h"

#include "loop.h"
#include "pins.h"

void loopTick(void) {
  HAL_GPIO_TogglePin(PIN_PERIPHERAL_LED, PIN_LED_DATA);
  HAL_Delay(200);
  uint8_t buffer[] = "Hello, World!\r\n";
  CDC_Transmit_FS(buffer, sizeof(buffer));
  HAL_Delay(1000);
}