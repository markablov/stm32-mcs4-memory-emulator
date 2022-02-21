#include "isrRingBuffer.h"

uint8_t isrRingBuffer[ISR_RING_BUFFER_LENGTH];

volatile uint16_t isrRingBufferWritePtr = 0;
volatile uint16_t isrRingBufferReadPtr = 0;