#ifndef ADC_MANAGER_H
#define ADC_MANAGER_H

#include "main.h"
#include <stdint.h>

/*
 * ============================================================================
 * ADC CHANNEL MAP
 * ============================================================================
 *
 * This enum defines the index position inside adcRaw[] buffer.
 *
 * IMPORTANT:
 * The order MUST match the ADC channel order configured in CubeMX.
 *
 * Example:
 * adcRaw[0] -> ADC_CH_IS_H1
 * adcRaw[1] -> ADC_CH_IS_H2
 * etc...
 *
 */

typedef enum
{
    ADC_CH_IS_H1 = 0,
    ADC_CH_IS_H2,
    ADC_CH_IS_H3,

    ADC_CH_IS_M1,
    ADC_CH_IS_M2,

    ADC_CH_IS_L1_2,
    ADC_CH_IS_L3_4,

    ADC_CH_AN_IN1,
    ADC_CH_AN_IN2,
    ADC_CH_AN_IN3,
    ADC_CH_AN_IN4,

    ADC_CH_BATTERY_SENSE

} ADC_Channel_t;


/*
 * ============================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================
 */

/*
 * Returns raw ADC value (0-4095)
 */
uint16_t ADC_GetRawValue(ADC_Channel_t channel);

/*
 * Returns ADC voltage converted from raw value
 */
float ADC_GetVoltage(ADC_Channel_t channel);

/*
 * Returns measured battery voltage
 */
float ADC_GetBatteryVoltage(void);

#endif
