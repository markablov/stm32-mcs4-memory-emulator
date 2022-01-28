#include "main.h"
#include "i4004Interface.h"

uint8_t i4004_readDataBus() {
  // for optimization pins are sequential 0..3
  return (IN_4004_D0_GPIO_Port->IDR & 0b0000000000001111U);
}

uint8_t i4004_readCMRAM() {
  // for optimization pins are sequential 4..7
  return (IN_4004_CMRAM0_GPIO_Port->IDR & 0b0000000011110000U) >> 4U;
}

uint8_t i4004_readCMROM() {
  return IN_4004_CMROM_GPIO_Port->IDR & IN_4004_CMROM_Pin;
}

uint8_t i4004_readSync() {
  return IN_4004_SYNC_GPIO_Port->IDR & IN_4004_SYNC_Pin;
}
