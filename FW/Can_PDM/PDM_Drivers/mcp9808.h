#ifndef INC_MCP9808_H_
#define INC_MCP9808_H_

#include "main.h"
#include <stdbool.h>

HAL_StatusTypeDef MCP9808_ReadTemperature(float *temperature);

#endif /* INC_MCP9808_H_ */
