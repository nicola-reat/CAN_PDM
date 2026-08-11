#include "mcp9808.h"
#include "i2c.h"

#define MCP9808_ADDR        (0x18 << 1)
#define MCP9808_TEMP_REG    0x05

HAL_StatusTypeDef MCP9808_ReadTemperature(float *temperature)
{
    uint8_t rxData[2];
    uint16_t rawTemp;
    HAL_StatusTypeDef status;

    // Read temperature register
    status = HAL_I2C_Mem_Read(&hi2c1,MCP9808_ADDR,MCP9808_TEMP_REG,I2C_MEMADD_SIZE_8BIT,rxData,2,10);

    // I2C communication error
    if (status != HAL_OK)
    {
        return status;
    }

    // Combine bytes
    rawTemp = ((uint16_t)rxData[0] << 8) | rxData[1];

    // Clear sign and flag bits
    rawTemp &= 0x0FFF;

    // Convert to Celsius
    *temperature = rawTemp / 16.0f;

    return HAL_OK;
}
