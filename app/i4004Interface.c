#include "main.h"
#include "i4004Interface.h"

volatile uint8_t i4004_readDataBus() {
  // for optimization pins are sequential 0..3
  return (IN_4004_D0_GPIO_Port->IDR & 0b0000000000001111U);
}

void i4004_writeDataBus(uint8_t data) {
  // for optimization pins are sequential 3..7 (4 data pins + enable-bus-write)
  // also need to invert 4-bit data word, because data bus is active low
  OUT_4004_D0_GPIO_Port->BSRR = (uint32_t)(data << 19U) | OUT_4004_ENABLE_BUS_WRITE_Pin | (uint32_t)((uint8_t)(data ^ 0xFU) << 3U);
}

void i4004_freeDataBus() {
  OUT_4004_ENABLE_BUS_WRITE_GPIO_Port->BSRR = (uint32_t)OUT_4004_ENABLE_BUS_WRITE_Pin << 16U;
}

volatile uint8_t i4004_readCMRAM() {
  // for optimization pins are sequential 4..7
  return (IN_4004_CMRAM0_GPIO_Port->IDR & 0b0000000011110000U) >> 4U;
}

volatile uint8_t i4004_readCMROM() {
  return (IN_4004_CMROM_GPIO_Port->IDR & IN_4004_CMROM_Pin) != 0;
}

volatile uint8_t i4004_readSync() {
  return (IN_4004_SYNC_GPIO_Port->IDR & IN_4004_SYNC_Pin) != 0;
}
