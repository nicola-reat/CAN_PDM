#include "profet.h"
#include "adc_manager.h"
#include "pdm_config.h"

/*
 * ============================================================================
 * PROFET CURRENT CALCULATION
 * ============================================================================
 *
 * PROFET current sense principle:
 *
 * Iload = IIS * KILIS
 *
 * IIS generates a voltage across Rsense.
 *
 * ADC measures:
 *
 * V = IIS * Rsense
 *
 * Therefore:
 *
 * IIS = V / Rsense
 *
 * Final formula:
 *
 * Iload = (V / Rsense) * KILIS
 *
 */


/*
 * ============================================================================
 * Profet_SetState
 * ============================================================================
 */

void Profet_SetState(ProfetChannel_t *channel, bool state)
{
    //Prevent enable if faulted
	if(CHANNEL_HARD_FAULT(channel) && state)
	{
	    return;
	}

    //Detect OFF -> ON transition
    if(state && !channel->enabled)
    {
        channel->enableTimestamp = HAL_GetTick();
    }

    channel->enabled = state;

    if(state)
    {
        HAL_GPIO_WritePin(
            channel->enablePort,
            channel->enablePin,
            GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(
            channel->enablePort,
            channel->enablePin,
            GPIO_PIN_RESET);
    }
}


/*
 * ============================================================================
 * Profet_UpdateCurrent
 * ============================================================================
 */

void Profet_UpdateCurrent(ProfetChannel_t *channel)
{
    float senseVoltage;
    float iis;
    float current;

    senseVoltage = ADC_GetVoltage(channel->adcChannel); //Read PROFET sense voltage
    iis = senseVoltage / channel->rSense; //Calculate IIS current
    current = iis * channel->kilis; //Calculate load current
    channel->current_mA = current * 1000.0f; // Convert to mA
}

/*
 * ============================================================================
 * Profet_GetTotalCurrent_mA
 * ============================================================================
 */

float Profet_GetTotalCurrent_mA(void)
{
    float total = 0;

    for(uint8_t i = 0; i < NUM_Profet_OUTPUTS; i++)
    {
        total += profetChannels[i]->current_mA;
    }

    return total;
}

/*
 * ============================================================================
 * Profet_CheckFaults
 * ============================================================================
 */
void Profet_CheckFaults(ProfetChannel_t *channel)
{
    float currentLimit;

    //Skip if already hard-faulted
    if(CHANNEL_HARD_FAULT(channel))
    {
        return;
    }

    //Recover from open load
    if(channel->faultType == FAULT_OPEN_LOAD &&
       channel->current_mA > channel->openLoadThreshold_mA)
    {
        channel->faultType = FAULT_NONE;

        //Restart startup phase
        channel->enableTimestamp = HAL_GetTick();
    }

    //Use startup current limit during inrush phase
    if((HAL_GetTick() - channel->enableTimestamp) < channel->startupTime_ms)
    {
        currentLimit = channel->startupMaxCurrent_mA;
    }
    else
    {
        currentLimit = channel->maxCurrent_mA;
    }

    //Short circuit protection
    if(channel->current_mA > channel->shortCircuitCurrent_mA)
    {
        channel->faultType = FAULT_SHORT_CIRCUIT;

        Profet_SetState(channel, false);

        return;
    }

    //Overcurrent protection
    if(channel->current_mA > currentLimit)
    {
        channel->faultType = FAULT_OVERCURRENT;

        Profet_SetState(channel, false);

        return;
    }

    //Open load detection
    if((HAL_GetTick() - channel->enableTimestamp) > 500)
    {
        if(channel->enabled &&
           channel->current_mA < channel->openLoadThreshold_mA)
        {
            channel->faultType = FAULT_OPEN_LOAD;

            return;
        }
    }
}

/*
 * ============================================================================
 * Profet_ResetFault
 * ============================================================================
 */
void Profet_ResetFault(ProfetChannel_t *channel)
{
    channel->faultType = FAULT_NONE;
}
