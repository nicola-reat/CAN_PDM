/*
 * io_expander.c
 *
 *  Created on: 17 giu 2026
 *      Author: Reatn
 */

#include "io_expander.h"
#include "pcal6524.h"
#include <stdint.h>
#include "profet.h"
#include "pdm_config.h"

typedef struct
{
    uint8_t port;
    uint8_t pin;
} IO_Map_t;

typedef struct
{
    IO_LED_t green;
    IO_LED_t red;
} OutputLedMap_t;


/*
 * ============================================================================
 * PRIVATE VARIABLES
 * ============================================================================
 */
//Timer used to update fault blink patterns.
static uint32_t blinkTimer = 0;

//Current step of the fault blink sequence.
static uint8_t blinkStep = 0;

//LED to PCAL6524 port/pin mapping.
static const IO_Map_t ledMap[IO_LED_COUNT] =
{
    // Port 0
    [IO_LED_DIN4]         = {0, 0},
    [IO_LED_DIN3]         = {0, 1},
    [IO_LED_DIN2]         = {0, 2},
    [IO_LED_DIN1]         = {0, 3},
    [IO_LED_L4_RED]       = {0, 4},
    [IO_LED_L4_GREEN]     = {0, 5},
    [IO_LED_L3_RED]       = {0, 6},
    [IO_LED_L3_GREEN]     = {0, 7},

    // Port 1
    [IO_LED_L2_RED]       = {1, 0},
    [IO_LED_L2_GREEN]     = {1, 1},
    [IO_LED_L1_RED]       = {1, 2},
    [IO_LED_L1_GREEN]     = {1, 3},
    [IO_LED_M2_RED]       = {1, 4},
    [IO_LED_M2_GREEN]     = {1, 5},
    [IO_LED_M1_RED]       = {1, 6},
    [IO_LED_M1_GREEN]     = {1, 7},

    // Port 2
    [IO_LED_H3_RED]       = {2, 0},
    [IO_LED_H3_GREEN]     = {2, 1},
    [IO_LED_H2_RED]       = {2, 2},
    [IO_LED_H2_GREEN]     = {2, 3},
    [IO_LED_H1_RED]       = {2, 4},
    [IO_LED_H1_GREEN]     = {2, 5},
    [IO_LED_DEBUG_BLUE]   = {2, 6},
    [IO_LED_DEBUG_YELLOW] = {2, 7}
};

static const OutputLedMap_t outputLedMap[NUM_Profet_OUTPUTS] =
{
    {IO_LED_H1_GREEN, IO_LED_H1_RED},
    {IO_LED_H2_GREEN, IO_LED_H2_RED},
    {IO_LED_H3_GREEN, IO_LED_H3_RED},
    {IO_LED_M1_GREEN, IO_LED_M1_RED},
    {IO_LED_M2_GREEN, IO_LED_M2_RED},
    {IO_LED_L1_GREEN, IO_LED_L1_RED},
    {IO_LED_L2_GREEN, IO_LED_L2_RED},
    {IO_LED_L3_GREEN, IO_LED_L3_RED},
    {IO_LED_L4_GREEN, IO_LED_L4_RED}
};
//Current state of each LED.
static bool ledState[IO_LED_COUNT];

/*
 * ============================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================
 */

//Initialize the PCAL6524 and clear all LED states.
void IOExpander_Init(void)
{
    PCAL6524_Init();

    for(uint8_t i = 0; i < IO_LED_COUNT; i++)
    {
        ledState[i] = false;
    }
}

//Set the specified LED state.
void IOExpander_SetLed(IO_LED_t led, bool state)
{
    if(led >= IO_LED_COUNT)
    {
        return;
    }

    ledState[led] = state;

    PCAL6524_WritePin(ledMap[led].port,ledMap[led].pin,state);
}

//Toggle the specified LED state.
void IOExpander_ToggleLed(IO_LED_t led)
{
    if(led >= IO_LED_COUNT)
    {
        return;
    }

    ledState[led] = !ledState[led];

    PCAL6524_WritePin(ledMap[led].port,ledMap[led].pin,ledState[led]);
}

//Return the current state of the specified LED.
bool IOExpander_GetLed(IO_LED_t led)
{
    if(led >= IO_LED_COUNT)
    {
        return false;
    }

    return ledState[led];
}

//Return the number of red LED blinks associated with a fault type.
static uint8_t IOExpander_GetFaultBlinkCount(ProfetFault_t fault)
{
    switch(fault)
    {
        case FAULT_OPEN_LOAD:
            return 1;

        case FAULT_OVERCURRENT:
            return 2;

        case FAULT_SHORT_CIRCUIT:
            return 3;

        case FAULT_THERMAL:
            return 4;

        case FAULT_UNDERVOLTAGE:
            return 5;

        default:
            return 0;
    }
}

//Update all output LEDs according to PROFET state and fault conditions.
void IOExpander_Update(void)
{
    //Update fault blink sequence every 200 ms.
    if(HAL_GetTick() - blinkTimer >= 200)
    {
        blinkTimer = HAL_GetTick();
        blinkStep++;
    }

    for(uint8_t i = 0; i < NUM_Profet_OUTPUTS; i++)
    {
        ProfetChannel_t *channel = profetChannels[i];

        //Fault condition.
        if(CHANNEL_FAULTED(channel))
        {
            //Get the number of blinks associated with the current fault.
            uint8_t blinkCount =IOExpander_GetFaultBlinkCount(channel->faultType);

            //Calculate total sequence length including pause time.
            uint8_t maxStep = (blinkCount * 2U) + 5U;

            //Restart blink sequence.
            if(blinkStep >= maxStep)
            {
                blinkStep = 0;
            }

            //Current red LED state.
            bool redState = false;

            //Generate the requested number of red LED blinks.
            if(blinkStep < (blinkCount * 2U))
            {
                redState = ((blinkStep % 2U) == 0U);
            }

            //Green OFF, red blinking.
            IOExpander_SetLed(outputLedMap[i].green, false);
            IOExpander_SetLed(outputLedMap[i].red, redState);
        }

        //Output enabled.
        else if(channel->enabled)
        {
            //Green ON, red OFF.
            IOExpander_SetLed(outputLedMap[i].green, true);
            IOExpander_SetLed(outputLedMap[i].red, false);
        }

        //Output disabled.
        else
        {
            //Both LEDs OFF.
            IOExpander_SetLed(outputLedMap[i].green, false);
            IOExpander_SetLed(outputLedMap[i].red, false);
        }
    }
}
