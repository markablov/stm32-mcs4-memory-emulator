#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "main.h"
#include "usart.h"
#include "commands.h"

#include "externalInterface.h"

#define MAX_OUTPUT_MSG_LEN 100
#define MAX_INPUT_MSG_LEN 100

#define UART_TRANSMIT_TIMEOUT 100

void initExternalInterface() {
  __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
}

void sendExternalBytes(uint8_t *data, uint16_t len) {
  HAL_UART_Transmit(&huart3, data, len, UART_TRANSMIT_TIMEOUT);
}

void sendExternalMessage(const char *format, ...) {
  va_list args;
  va_start(args, format);

  char buf[MAX_OUTPUT_MSG_LEN];

  vsnprintf(buf, sizeof(buf), format, args);
  sendExternalBytes((uint8_t *)buf, (uint16_t)strlen(buf));

  va_end(args);
}

/*
 *  INPUT PROCESSING
 */

enum InputStage { InputCommand, InputDataLength, InputData };
enum InputCommand { CmdNone, CmdStart, CmdStop };

static enum InputStage inputStage = InputCommand;
static enum InputCommand inputCommand = CmdNone;
static uint8_t input[MAX_INPUT_MSG_LEN];
static uint8_t inputPos = 0;
static uint8_t *inputData = NULL;
static uint32_t inputDataLen = 0;
static uint32_t inputDataPos = 0;

void processInputSymbol(uint8_t symbol) {
  uint8_t cmdFinished = 0;

  switch (inputStage) {
    case InputCommand:
      if (symbol == '\n') {
        input[inputPos] = 0;
        inputPos = 0;

        if (strcmp((const char *)input, "START") == 0) {
          inputCommand = CmdStart;
          inputStage = InputDataLength;
        }
        if (strcmp((const char *)input, "STOP") == 0) {
          inputCommand = CmdStop;
          cmdFinished = 1;
        }
      } else {
        if (symbol != '\r') {
          input[inputPos] = symbol;
          inputPos++;
          if (inputPos >= MAX_INPUT_MSG_LEN) {
            inputPos = 0;
          }
        }
      }
      break;

    case InputDataLength:
      input[inputPos] = symbol;
      inputPos++;
      if (inputPos == 4) {
        inputPos = 0;
        inputDataLen = *(uint32_t *)input;
        if (inputDataLen == 0) {
          cmdFinished = 1;
        } else {
          inputDataPos = 0;
          inputData = malloc((size_t)inputDataLen);
          inputStage = InputData;
        }
      }
      break;

    case InputData:
      inputData[inputDataPos] = symbol;
      inputDataPos++;
      if (inputDataPos == inputDataLen) {
        cmdFinished = 1;
      }
      break;
    default:
      break;
  }

  if (cmdFinished == 1) {
    if (inputCommand == CmdStart) {
      cmdStart(inputData, inputDataLen);
    } else if (inputCommand == CmdStop) {
      cmdStop();
    }

    if (inputData) {
      free(inputData);
      inputData = NULL;
    }

    // wait for new command
    inputStage = InputCommand;
  }
}
