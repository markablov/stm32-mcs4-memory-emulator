#include "main.h"
#include "tim.h"
#include "init.h"

void init(void) {
  // Timers to generate clock signals for MCU-4 processor
  HAL_TIM_Base_Start(&htim8);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);

  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);
}