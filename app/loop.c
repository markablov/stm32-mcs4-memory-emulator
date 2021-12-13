#include <stdio.h>
#include "main.h"

#include "loop.h"
#include "pins.h"

void loopTick(void) {
  HAL_GPIO_TogglePin(PIN_PERIPHERAL_LED, PIN_LED_DATA);
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
  HAL_Delay(200);
}