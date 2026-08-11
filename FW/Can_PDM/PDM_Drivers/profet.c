#include "profet.h"
#include "adc_manager.h"
#include "pdm_config.h"
volatile float debug_senseVoltage;
volatile float debug_iis;
volatile float debug_current;
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


    debug_senseVoltage = senseVoltage;
    debug_iis = iis;
    debug_current = current;
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
 * PROFET Fault Monitoring Summary
 * ============================================================================
 *
 * Fault monitoring is performed only when the output is enabled.
 *
 * Output OFF:
 *   - No current monitoring is performed.
 *   - Short-circuit timer is reset.
 *   - Open-load timer is reset.
 *   - This prevents false faults caused by ADC noise or residual current
 *     readings while the PROFET is disabled.
 *
 * Output ON:
 *   - Startup current limit is applied during the configured startup time.
 *   - Short circuit is detected if current remains above the short-circuit
 *     threshold for at least 50 ms.
 *   - Overcurrent is detected if current exceeds the active current limit.
 *   - Open load is detected if current remains below the open-load threshold
 *     for at least 200 ms after the initial 500 ms activation delay.
 *   - An Open Load fault is cleared when the current returns above the
 *     open-load threshold.
 *
 * When a critical fault is confirmed, the PROFET output is switched OFF.
 * ============================================================================
 */

void Profet_CheckFaults(ProfetChannel_t *channel)
{
    float currentLimit;

    // ========================================================================
    // Skip fault monitoring if the channel is already in a hard fault state
    // ========================================================================
    if(CHANNEL_HARD_FAULT(channel))
    {
        return;
    }

    // ========================================================================
    // Output OFF:
    // Do not perform any current-based fault monitoring.
    //
    // The current ADC reading may contain residual voltage or noise while
    // the PROFET is disabled. Monitoring the current in this state could
    // therefore generate false overcurrent, short-circuit or open-load faults.
    // ========================================================================
    if(!channel->enabled)
    {
        // Reset fault timers while the output is disabled.
        channel->shortCircuitTimestamp = 0;
        channel->openLoadTimestamp = 0;

        return;
    }

    // ========================================================================
    // Output ON:
    // From this point onward the channel is enabled and current monitoring
    // can safely be performed.
    // ========================================================================


    // ========================================================================
    // Recover from OPEN LOAD
    //
    // If an open-load fault was previously detected and the current has
    // returned above the open-load threshold, clear the fault.
    // ========================================================================
    if(channel->faultType == FAULT_OPEN_LOAD &&
       channel->current_mA > channel->openLoadThreshold_mA)
    {
        channel->faultType = FAULT_NONE;

        // Restart the startup monitoring phase
        channel->enableTimestamp = HAL_GetTick();

        // Reset the open-load timer
        channel->openLoadTimestamp = 0;
    }


    // ========================================================================
    // Determine the active current limit
    //
    // During startup, a higher current limit is allowed to handle inrush
    // current. After startup, the normal maximum current limit is used.
    // ========================================================================
    if((HAL_GetTick() - channel->enableTimestamp) < channel->startupTime_ms)
    {
        // Startup current limit
        currentLimit = channel->startupMaxCurrent_mA;
    }
    else
    {
        // Normal operating current limit
        currentLimit = channel->maxCurrent_mA;
    }


    // ========================================================================
    // Short-circuit protection
    //
    // A short circuit is not triggered immediately.
    // The current must remain above the short-circuit threshold for 50 ms.
    // ========================================================================
    if(channel->current_mA > channel->shortCircuitCurrent_mA)
    {
        // Start the short-circuit timer
        if(channel->shortCircuitTimestamp == 0)
        {
            channel->shortCircuitTimestamp = HAL_GetTick();
        }

        // Confirm short circuit only if the condition remains for 50 ms
        if((HAL_GetTick() - channel->shortCircuitTimestamp) >= 50)
        {
            channel->faultType = FAULT_SHORT_CIRCUIT;

            // Reset the timer
            channel->shortCircuitTimestamp = 0;

            // Disable the PROFET output
            Profet_SetState(channel, false);

            return;
        }
    }
    else
    {
        // Current returned below the short-circuit threshold
        // Cancel the short-circuit timer
        channel->shortCircuitTimestamp = 0;
    }


    // ========================================================================
    // Overcurrent protection
    //
    // If the current exceeds the currently active current limit,
    // immediately disable the output and report an overcurrent fault.
    // ========================================================================
    if(channel->current_mA > currentLimit)
    {
        channel->faultType = FAULT_OVERCURRENT;

        // Disable the PROFET output
        Profet_SetState(channel, false);

        return;
    }


    // ========================================================================
    // Open-load detection
    //
    // Open-load monitoring starts only after the initial 500 ms startup
    // period. This prevents false open-load detection during activation.
    // ========================================================================
    if((HAL_GetTick() - channel->enableTimestamp) > 500)
    {
        // Output is enabled and current is below the minimum expected load
        if(channel->current_mA < channel->openLoadThreshold_mA)
        {
            // Start the open-load timer
            if(channel->openLoadTimestamp == 0)
            {
                channel->openLoadTimestamp = HAL_GetTick();
            }

            // Confirm open load only if the condition remains for 200 ms
            if((HAL_GetTick() - channel->openLoadTimestamp) >= 200)
            {
                channel->faultType = FAULT_OPEN_LOAD;

                return;
            }
        }
        else
        {
            // Current returned to normal
            // Cancel the open-load timer
            channel->openLoadTimestamp = 0;
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
