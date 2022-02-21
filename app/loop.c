#include "main.h"
#include "isrRingBuffer.h"
#include "externalInterface.h"

#include "loop.h"

void sendDataThroughUARTIfNecessary() {
  // save in function context, to avoid race condition with volatile variables
  uint16_t writePtr = isrRingBufferWritePtr;
  uint16_t readPtr = isrRingBufferReadPtr;

  if (readPtr == writePtr) {
    return;
  }

  uint16_t len = writePtr > readPtr ? writePtr - readPtr : ISR_RING_BUFFER_LENGTH - readPtr;
  sendExternalBytes(&isrRingBuffer[readPtr], len);
  isrRingBufferReadPtr = (readPtr + len) % ISR_RING_BUFFER_LENGTH;
}

static uint32_t lastToggleTick = 0;

void loopTick(void) {
  sendDataThroughUARTIfNecessary();

  // flash LED
  if (!lastToggleTick || (HAL_GetTick() - lastToggleTick) > 1000) {
    HAL_GPIO_TogglePin(OUT_LED_DATA_GPIO_Port, OUT_LED_DATA_Pin);
    lastToggleTick = HAL_GetTick();
  }
}
