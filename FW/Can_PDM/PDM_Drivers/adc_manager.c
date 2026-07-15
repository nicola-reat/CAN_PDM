#include "adc_manager.h"
#include "adc.h"
#include "pdm_config.h"

/*
 * ============================================================================
 * EXTERNAL ADC DMA BUFFER
 * ============================================================================
 *
 * This buffer is continuously updated by ADC + DMA.
 *
 * Declared in adc.c
 *
 */

extern uint16_t adcRaw[12];


/*
 * ============================================================================
 * ADC_GetRawValue
 * ============================================================================
 *
 * Returns raw ADC value from DMA buffer.
 *
 */

uint16_t ADC_GetRawValue(ADC_Channel_t channel)
{
    return adcRaw[channel];
}


/*
 * ============================================================================
 * ADC_GetVoltage
 * ============================================================================
 *
 * Converts ADC raw value to voltage.
 *
 * Formula:
 *
 * voltage = raw / 4095 * 3.3
 *
 */

float ADC_GetVoltage(ADC_Channel_t channel)
{
    float voltage;

    voltage = ((float)adcRaw[channel] / 4095.0f) * 3.3f;

    return voltage;
}


/*
 * ============================================================================
 * ADC_GetBatteryVoltage
 * ============================================================================
 *
 * Converts battery sense ADC voltage to REAL battery voltage.
 *

 */

float ADC_GetBatteryVoltage(void)
{
    float adcVoltage;
    float batteryVoltage;

    /*
     * Read ADC pin voltage
     */
    adcVoltage = ADC_GetVoltage(ADC_CH_BATTERY_SENSE);

    /*
     * Convert ADC voltage to real battery voltage
     * using resistor divider ratio.
     */
    batteryVoltage = adcVoltage * PDM_BATTERY_DIVIDER_RATIO;

    return batteryVoltage;
}
