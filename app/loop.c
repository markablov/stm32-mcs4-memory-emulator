#include <stdio.h>
#include "main.h"

#include "loop.h"

void loopTick(void) {
  HAL_GPIO_TogglePin(OUT_LED_DATA_GPIO_Port, OUT_LED_DATA_Pin);
  HAL_Delay(200);
}