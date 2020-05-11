#include "main.h"
#include "tim.h"
#include "init.h"

void init(void) {
  // Timers to generate clock signals for MCU-4 processor
  HAL_TIM_Base_Start(&htim9);
  HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
}