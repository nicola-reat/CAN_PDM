/*
 * io_expander.h
 *
 *  Created on: 17 giu 2026
 *      Author: Reatn
 */

#ifndef INC_IO_EXPANDER_H_
#define INC_IO_EXPANDER_H_

#include <stdbool.h>

/*
 * ============================================================================
 * LED ENUMERATION
 * ============================================================================
 */

typedef enum
{
    // Port 0
    IO_LED_DIN4,
    IO_LED_DIN3,
    IO_LED_DIN2,
    IO_LED_DIN1,
    IO_LED_L4_RED,
    IO_LED_L4_GREEN,
    IO_LED_L3_RED,
    IO_LED_L3_GREEN,

    // Port 1
    IO_LED_L2_RED,
    IO_LED_L2_GREEN,
    IO_LED_L1_RED,
    IO_LED_L1_GREEN,
    IO_LED_M2_RED,
    IO_LED_M2_GREEN,
    IO_LED_M1_RED,
    IO_LED_M1_GREEN,

    // Port 2
    IO_LED_H3_RED,
    IO_LED_H3_GREEN,
    IO_LED_H2_RED,
    IO_LED_H2_GREEN,
    IO_LED_H1_RED,
    IO_LED_H1_GREEN,
    IO_LED_DEBUG_BLUE,
    IO_LED_DEBUG_YELLOW,

    IO_LED_COUNT

} IO_LED_t;

/*
 * ============================================================================
 * FUNCTIONS
 * ============================================================================
 */

//Initialize the PCAL6524 and turn off all LEDs.
void IOExpander_Init(void);

//Set the specified LED state.
void IOExpander_SetLed(IO_LED_t led, bool state);

//Toggle the specified LED state.
void IOExpander_ToggleLed(IO_LED_t led);

//Get the current state of the specified LED.
bool IOExpander_GetLed(IO_LED_t led);

//Update led output state
void IOExpander_Update(void);
#endif /* INC_IO_EXPANDER_H_ */
