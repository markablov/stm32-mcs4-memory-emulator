#include "main.h"
#include "tim.h"
#include "init.h"
#include "externalInterface.h"

/*
 * For MCS-4 processor we need to provide two-phase clock with period 1350ns (targetFreq = 741kHz) and width 380ns (active LOW)
 * Distance between two rising (and falling) edges should be: 780ns or 530ns (depends on which clock signal is chosen as first)
 *
 * To achieve that master-slave timers are used with same period (frequency): TIM_Period = (timerFreq / targetFreq) - 1 = 168Mhz / 0.741Mhz - 1 = 226
 * Pulse width should be (TIM_Period + 1) * (targetPeriod / timerPeriod) - 1 = 227 * (970ms / 1350ms) - 1 = 162
 * Phase offset should be (TIM_Period + 1) * (targetOffset / timerPeriod) - 1 = 227 * (530ms / 1350ms) - 1 = 88
 *
 * Interrupts:
 *   TIM1 Update - phi1 raising
 *   TIM1 CC - phi1 falling
 *   TIM8 Update - phi2 raising
 *   TIM8 CC - phi2 falling
 *
 * From oscilloscope data, i4004 updates some signals (CM-RAM / CM-ROM) at phi2 falling (from high to low), so we need to have interrupt here
 */
void initMCS4Clocks() {
  HAL_TIM_Base_Start(&htim8);
  HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_4);
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);
}

void init(void) {
  initMCS4Clocks();
  initExternalInterface();
}