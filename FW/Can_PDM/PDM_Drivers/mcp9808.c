#include "mcp9808.h"
#include "i2c.h"

#define MCP9808_ADDR    (0x18 << 1)
#define MCP9808_TEMP_REG 0x05

float MCP9808_ReadTemperature(void)
{
    uint8_t rxData[2];

    uint16_t rawTemp;

    float temperature;

    //Read temperature register
    HAL_I2C_Mem_Read(&hi2c1,MCP9808_ADDR,MCP9808_TEMP_REG,I2C_MEMADD_SIZE_8BIT,rxData,2,100);

    //Combine bytes
    rawTemp = (rxData[0] << 8) | rxData[1];

    //Clear flags bits
    rawTemp &= 0x0FFF;

    //Convert to temperature
    temperature = rawTemp & 0x0FFF;

    temperature /= 16.0f;

    return temperature;
}
