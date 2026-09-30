#ifndef MCP9808_H
#define MCP9808_H

#include "main.h"
#include "i2c.h"



/*
 * ============================================================================
 * MCP9808 I2C CONFIGURATION
 * ============================================================================
 *
 */

#define MCP9808_ADDR                    (0x18 << 1) //I2c address

#define MCP9808_UPPER_TEMP_LIMIT        80.0f //Upper temperature limit:The ALERT output is asserted when the temperature reaches this value.
#define MCP9808_CRITICAL_TEMP_LIMIT     100.0f //Critical temperature limit:This threshold is configured in the MCP9808 but is not used to control he external ALERT LED.
/*
 * Critical temperature limit:
 */


/*
 * ============================================================================
 * MCP9808 REGISTERS
 * ============================================================================
 */

#define MCP9808_REG_CONFIG              0x01
#define MCP9808_REG_UPPER_TEMP          0x02
#define MCP9808_REG_LOWER_TEMP          0x03
#define MCP9808_REG_CRITICAL_TEMP       0x04
#define MCP9808_REG_AMBIENT_TEMP        0x05
#define MCP9808_REG_MANUFACTURER_ID     0x06
#define MCP9808_REG_DEVICE_ID           0x07
#define MCP9808_REG_RESOLUTION          0x08

/*
 * ============================================================================
 * MCP9808 DEVICE IDENTIFICATION
 * ============================================================================
 */

#define MCP9808_MANUFACTURER_ID         0x0054
#define MCP9808_DEVICE_ID               0x0400

/*
 * ============================================================================
 * MCP9808 CONFIGURATION BITS
 * ============================================================================
 *
 * CONFIG register:
 *
 * Bit 2     ALERT output select
 * Bit 1     ALERT polarity
 * Bit 0     ALERT mode
 * Bit 6     TCRIT lock
 * Bit 7     TUPPER/TLOWER lock
 * Bit 8     Shutdown
 *
 */

/* ALERT output is driven by TUPPER/TLOWER comparison */
#define MCP9808_CONFIG_ALERT_SELECT     (0U << 2)

/* ALERT active low */
#define MCP9808_CONFIG_ALERT_POLARITY   (0U << 1)

/* Comparator mode */
#define MCP9808_CONFIG_ALERT_MODE       (0U << 0)

/*
 * ============================================================================
 * MCP9808 TEMPERATURE STATUS BITS
 * ============================================================================
 */

#define MCP9808_TEMP_CRITICAL           (1U << 15)
#define MCP9808_TEMP_UPPER              (1U << 14)
#define MCP9808_TEMP_LOWER              (1U << 13)

/*
 * ============================================================================
 * MCP9808 HYSTERESIS
 * ============================================================================
 *
 * TUPPER/TLOWER hysteresis:
 *
 * 00 = 0.0 °C
 * 01 = 1.5 °C
 * 10 = 3.0 °C
 * 11 = 6.0 °C
 *
 */

#define MCP9808_HYSTERESIS_0C           (0U << 9)
#define MCP9808_HYSTERESIS_1_5C         (1U << 9)
#define MCP9808_HYSTERESIS_3C           (2U << 9)
#define MCP9808_HYSTERESIS_6C           (3U << 9)

/*
 * ============================================================================
 * MCP9808 RESOLUTION
 * ============================================================================
 */

#define MCP9808_RESOLUTION_0_5C         0x00
#define MCP9808_RESOLUTION_0_25C        0x01
#define MCP9808_RESOLUTION_0_125C       0x02
#define MCP9808_RESOLUTION_0_0625C      0x03

/*
 * ============================================================================
 * MCP9808 STATUS
 * ============================================================================
 */

typedef struct
{
    uint8_t critical;
    uint8_t upper;
    uint8_t lower;

} MCP9808_Status_t;

/*
 * ============================================================================
 * MCP9808 FUNCTIONS
 * ============================================================================
 */

/*
 * Initialize MCP9808.
 *
 * Checks the device ID, configures the resolution, sets the temperature
 * limits and configures the ALERT output.
 *
 */
HAL_StatusTypeDef MCP9808_Init(float upperLimit, float criticalLimit);

/*
 * Check MCP9808 manufacturer and device ID.
 *
 */
HAL_StatusTypeDef MCP9808_CheckId(void);

/*
 * Read ambient temperature in degrees Celsius.
 *
 */
HAL_StatusTypeDef MCP9808_ReadTemperature(float *temperature);

/*
 * Set MCP9808 temperature resolution.
 *
 */
HAL_StatusTypeDef MCP9808_SetResolution(uint8_t resolution);

/*
 * Set upper temperature limit.
 *
 */
HAL_StatusTypeDef MCP9808_SetUpperLimit(float temperature);

/*
 * Set critical temperature limit.
 *
 */
HAL_StatusTypeDef MCP9808_SetCriticalLimit(float temperature);

/*
 * Read MCP9808 temperature status flags.
 *
 */
HAL_StatusTypeDef MCP9808_GetStatus(MCP9808_Status_t *status);

#endif


