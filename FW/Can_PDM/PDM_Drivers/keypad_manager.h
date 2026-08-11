#ifndef INC_KEYPAD_MANAGER_H_
#define INC_KEYPAD_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

#include "profet.h"

typedef struct
{
    uint8_t keyNumber;
    ProfetChannel_t *channel;
    bool lastState;
    uint8_t lastLedMode;
    uint8_t lastLedState;
    uint32_t lastLedUpdate;
} KeypadButton_t;


typedef enum
{
    KEYPAD_LED_OFF     		 = 0x00,
    KEYPAD_LED_RED     		 = 0x01,
    KEYPAD_LED_GREEN   		 = 0x02,
    KEYPAD_LED_BLUE    		 = 0x03,
    KEYPAD_LED_YELLOW  		 = 0x04,
    KEYPAD_LED_MAGENTA  	 = 0x05,
    KEYPAD_LED_CYAN    		 = 0x06,
    KEYPAD_LED_WHITE   		 = 0x07,
	KEYPAD_LED_AMBER_ORANGE  = 0x08

} KeypadLedColor_t;

//Keypad led mode
typedef enum
{
    KEYPAD_LED_MODE_OFF   = 0x00,
    KEYPAD_LED_MODE_ON    = 0x01,
    KEYPAD_LED_MODE_BLINK = 0x02

} KeypadLedMode_t;

void Keypad_Init(void);
void Keypad_ProcessMessage(uint8_t keyNumber,bool pressed);
void Keypad_UpdateLEDs(void);

#endif
