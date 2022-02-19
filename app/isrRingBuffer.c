#include "isrRingBuffer.h"

uint8_t isrRingBuffer[ISR_RING_BUFFER_LENGTH];

volatile uint8_t isrRingBufferWritePtr = 0;
volatile uint8_t isrRingBufferReadPtr = 0;