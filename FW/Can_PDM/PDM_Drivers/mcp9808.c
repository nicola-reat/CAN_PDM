#include "mcp9808.h"
#include "i2c.h"

/*
 * ============================================================================
 * MCP9808 INITIALIZATION
 * ============================================================================
 *
 * Initializes the MCP9808 temperature sensor.
 *
 * The initialization sequence is:
 *
 * 1. Check manufacturer and device ID
 * 2. Set temperature resolution
 * 3. Set upper temperature limit
 * 4. Set critical temperature limit
 * 5. Configure ALERT output
 *
 * ALERT is handled completely by the MCP9808 hardware.
 *
 * The LED connected to ALERT will:
 *
 * - Turn ON when temperature reaches TUPPER
 * - Remain ON while temperature is above the hysteresis threshold
 * - Turn OFF when temperature falls below TUPPER - 1.5 °C
 *
 */
HAL_StatusTypeDef MCP9808_Init(float upperLimit, float criticalLimit)
{
    HAL_StatusTypeDef status;
    uint16_t config;

    /*
     * ========================================================================
     * CHECK DEVICE ID
     * ========================================================================
     */

    status = MCP9808_CheckId();

    if (status != HAL_OK)
        return status;

    /*
     * ========================================================================
     * SET TEMPERATURE RESOLUTION
     * ========================================================================
     *
     * Maximum resolution:
     *
     * 0.0625 °C
     *
     */

    status = MCP9808_SetResolution(MCP9808_RESOLUTION_0_0625C);

    if (status != HAL_OK)
        return status;

    /*
     * ========================================================================
     * SET UPPER TEMPERATURE LIMIT
     * ========================================================================
     */

    status = MCP9808_SetUpperLimit(upperLimit);

    if (status != HAL_OK)
        return status;

    /*
     * ========================================================================
     * SET CRITICAL TEMPERATURE LIMIT
     * ========================================================================
     */

    status = MCP9808_SetCriticalLimit(criticalLimit);

    if (status != HAL_OK)
        return status;

    /*
     * ========================================================================
     * CONFIGURE ALERT OUTPUT
     * ========================================================================
     *
     * ALERT configuration:
     *
     * - TUPPER/TLOWER comparison
     * - Active low
     * - Comparator mode
     * - 1.5 °C hysteresis
     *
     * TCRIT is not used to control the LED.
     *
     */

    config = MCP9808_CONFIG_ALERT_SELECT |
             MCP9808_CONFIG_ALERT_POLARITY |
             MCP9808_CONFIG_ALERT_MODE |
             MCP9808_HYSTERESIS_1_5C;

    /*
     * ========================================================================
     * WRITE CONFIGURATION REGISTER
     * ========================================================================
     */

    uint8_t txData[2];

    txData[0] = config >> 8;
    txData[1] = config & 0xFF;

    status = HAL_I2C_Mem_Write(&hi2c1,MCP9808_ADDR,MCP9808_REG_CONFIG,I2C_MEMADD_SIZE_8BIT,txData,2,10);

    if (status != HAL_OK)
        return status;

    return HAL_OK;
}

/*
 * ============================================================================
 * CHECK MCP9808 DEVICE ID
 * ============================================================================
 *
 * Reads the manufacturer ID and device ID registers.
 *
 * This verifies that the expected MCP9808 is present on the I2C bus.
 *
 */
HAL_StatusTypeDef MCP9808_CheckId(void)
{
    HAL_StatusTypeDef status;

    uint8_t rxData[2];

    uint16_t manufacturerId;
    uint16_t deviceId;

    /*
     * ========================================================================
     * READ MANUFACTURER ID
     * ========================================================================
     */

    status = HAL_I2C_Mem_Read(&hi2c1,MCP9808_ADDR,MCP9808_REG_MANUFACTURER_ID,I2C_MEMADD_SIZE_8BIT,rxData,2,10);

    if (status != HAL_OK)
        return status;

    manufacturerId = ((uint16_t)rxData[0] << 8) | rxData[1];

    if (manufacturerId != MCP9808_MANUFACTURER_ID)
        return HAL_ERROR;

    /*
     * ========================================================================
     * READ DEVICE ID
     * ========================================================================
     */

    status = HAL_I2C_Mem_Read(&hi2c1,MCP9808_ADDR,MCP9808_REG_DEVICE_ID,I2C_MEMADD_SIZE_8BIT,rxData,2,10);

    if (status != HAL_OK)
        return status;

    deviceId = ((uint16_t)rxData[0] << 8) | rxData[1];

    if (deviceId != MCP9808_DEVICE_ID)
        return HAL_ERROR;

    return HAL_OK;
}

/*
 * ============================================================================
 * READ AMBIENT TEMPERATURE
 * ============================================================================
 *
 * Reads the MCP9808 ambient temperature register and converts the raw value
 * into degrees Celsius.
 *
 */
HAL_StatusTypeDef MCP9808_ReadTemperature(float *temperature)
{
    HAL_StatusTypeDef status;

    uint8_t rxData[2];
    uint16_t rawTemp;

    /*
     * ========================================================================
     * READ TEMPERATURE REGISTER
     * ========================================================================
     */

    status = HAL_I2C_Mem_Read(&hi2c1,MCP9808_ADDR,MCP9808_REG_AMBIENT_TEMP,I2C_MEMADD_SIZE_8BIT,rxData,2,10);

    if (status != HAL_OK)
        return status;

    /*
     * ========================================================================
     * COMBINE RECEIVED BYTES
     * ========================================================================
     */

    rawTemp = ((uint16_t)rxData[0] << 8) | rxData[1];

    /*
     * ========================================================================
     * CONVERT RAW TEMPERATURE
     * ========================================================================
     *
     * Bit 12 indicates a negative temperature.
     *
     */

    rawTemp &= 0x1FFF;

    *temperature = (float)(rawTemp & 0x0FFF) / 16.0f;

    if (rawTemp & 0x1000)
    {
        *temperature -= 256.0f;
    }

    return HAL_OK;
}

/*
 * ============================================================================
 * SET TEMPERATURE RESOLUTION
 * ============================================================================
 *
 * Resolution:
 *
 * 0 = 0.5 °C
 * 1 = 0.25 °C
 * 2 = 0.125 °C
 * 3 = 0.0625 °C
 *
 */
HAL_StatusTypeDef MCP9808_SetResolution(uint8_t resolution)
{
    resolution &= 0x03;

    return HAL_I2C_Mem_Write(&hi2c1,MCP9808_ADDR,MCP9808_REG_RESOLUTION,I2C_MEMADD_SIZE_8BIT,&resolution,1,10);
}

/*
 * ============================================================================
 * SET UPPER TEMPERATURE LIMIT
 * ============================================================================
 *
 * Sets the temperature at which the ALERT output is asserted.
 *
 */
HAL_StatusTypeDef MCP9808_SetUpperLimit(float temperature)
{
    uint16_t rawTemperature;
    uint8_t txData[2];

    /*
     * ========================================================================
     * CONVERT TEMPERATURE TO MCP9808 FORMAT
     * ========================================================================
     */

    rawTemperature = (uint16_t)(temperature * 16.0f);

    txData[0] = rawTemperature >> 8;
    txData[1] = rawTemperature & 0xFF;

    /*
     * ========================================================================
     * WRITE UPPER TEMPERATURE LIMIT
     * ========================================================================
     */

    return HAL_I2C_Mem_Write(&hi2c1,MCP9808_ADDR,MCP9808_REG_UPPER_TEMP,I2C_MEMADD_SIZE_8BIT,txData,2,10);
}

/*
 * ============================================================================
 * SET CRITICAL TEMPERATURE LIMIT
 * ============================================================================
 *
 * Sets the critical temperature threshold.
 *
 * The critical threshold is configured in the MCP9808 but is not used
 * to control the external LED.
 *
 */
HAL_StatusTypeDef MCP9808_SetCriticalLimit(float temperature)
{
    uint16_t rawTemperature;
    uint8_t txData[2];

    /*
     * ========================================================================
     * CONVERT TEMPERATURE TO MCP9808 FORMAT
     * ========================================================================
     */

    rawTemperature = (uint16_t)(temperature * 16.0f);

    txData[0] = rawTemperature >> 8;
    txData[1] = rawTemperature & 0xFF;

    /*
     * ========================================================================
     * WRITE CRITICAL TEMPERATURE LIMIT
     * ========================================================================
     */

    return HAL_I2C_Mem_Write(&hi2c1,MCP9808_ADDR,MCP9808_REG_CRITICAL_TEMP,I2C_MEMADD_SIZE_8BIT,txData,2,10);
}

/*
 * ============================================================================
 * READ MCP9808 TEMPERATURE STATUS
 * ============================================================================
 *
 * Reads the temperature register and extracts the internal temperature
 * comparison flags.
 *
 */
HAL_StatusTypeDef MCP9808_GetStatus(MCP9808_Status_t *status)
{
    HAL_StatusTypeDef result;

    uint8_t rxData[2];
    uint16_t rawTemp;

    /*
     * ========================================================================
     * READ TEMPERATURE REGISTER
     * ========================================================================
     */

    result = HAL_I2C_Mem_Read(&hi2c1,MCP9808_ADDR,MCP9808_REG_AMBIENT_TEMP,I2C_MEMADD_SIZE_8BIT,rxData,2,10);

    if (result != HAL_OK)
        return result;

    rawTemp = ((uint16_t)rxData[0] << 8) | rxData[1];

    /*
     * ========================================================================
     * EXTRACT TEMPERATURE STATUS FLAGS
     * ========================================================================
     */

    status->critical =
        (rawTemp & MCP9808_TEMP_CRITICAL) ? 1 : 0;

    status->upper =
        (rawTemp & MCP9808_TEMP_UPPER) ? 1 : 0;

    status->lower =
        (rawTemp & MCP9808_TEMP_LOWER) ? 1 : 0;

    return HAL_OK;
}

