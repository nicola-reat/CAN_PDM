#include "keypad_manager.h"
#include "can.h"
#include "pdm_config.h"

KeypadButton_t keypadButtons[9];
static void Keypad_SendLEDCommand(uint8_t keyNumber,uint8_t color,uint8_t mode);
static void Keypad_SetBrightness(uint8_t percent);

//Init Keypad
void Keypad_Init(void)
{
    /*
     * =========================================================================
     * Configure keypad button mapping
     * =========================================================================
     */
    if (PDM_ID == PDM_1)
    {
    // mapping PDM1
    //High outputs
    keypadButtons[0].keyNumber = 8;
    keypadButtons[0].channel = &profet_H1;

    keypadButtons[1].keyNumber = 14;
    keypadButtons[1].channel = &profet_H2;

    keypadButtons[2].keyNumber = 10 ;
    keypadButtons[2].channel = &profet_H3;

    //Medium outputs
    keypadButtons[3].keyNumber = 3;
    keypadButtons[3].channel = &profet_M1;

    keypadButtons[4].keyNumber = 2;
    keypadButtons[4].channel = &profet_M2;

    //Low outputs
    keypadButtons[5].keyNumber = 1;
    keypadButtons[5].channel = &profet_L1;

    keypadButtons[6].keyNumber = 30;
    keypadButtons[6].channel = &profet_L2;

    keypadButtons[7].keyNumber = 30 ;
    keypadButtons[7].channel = &profet_L3;

    keypadButtons[8].keyNumber = 30;
    keypadButtons[8].channel = &profet_L4;
    }else{
    // mapping PDM2
    //High outputs
    keypadButtons[0].keyNumber = 6;
    keypadButtons[0].channel = &profet_H1;

    keypadButtons[1].keyNumber = 7;
    keypadButtons[1].channel = &profet_H2;

    keypadButtons[2].keyNumber = 30 ;
    keypadButtons[2].channel = &profet_H3;

    //Medium outputs
    keypadButtons[3].keyNumber = 13;
    keypadButtons[3].channel = &profet_M1;

    keypadButtons[4].keyNumber = 30;
    keypadButtons[4].channel = &profet_M2;

    //Low outputs
    keypadButtons[5].keyNumber = 1;
    keypadButtons[5].channel = &profet_L1;

    keypadButtons[6].keyNumber = 5;
    keypadButtons[6].channel = &profet_L2;

    keypadButtons[7].keyNumber = 30 ;
    keypadButtons[7].channel = &profet_L3;

    keypadButtons[8].keyNumber = 30;
    keypadButtons[8].channel = &profet_L4;

    }
    /*
         * =========================================================================
         * Reset all keypad LEDs
         * =========================================================================
         */

        for(uint8_t i = 0; i < 9; i++)
        {
            //Turn LED OFF
            Keypad_SendLEDCommand(keypadButtons[i].keyNumber,KEYPAD_LED_OFF,KEYPAD_LED_MODE_OFF);
            //Save initial LED state
            keypadButtons[i].lastLedState = KEYPAD_LED_OFF;
            //Save initial LED mode
            keypadButtons[i].lastLedMode = KEYPAD_LED_MODE_OFF;
            //Initialize LED refresh timer
            keypadButtons[i].lastLedUpdate = HAL_GetTick();
            Keypad_SetBrightness(100); //Set brightness 100
        }
}

//Keypad activate infineon
void Keypad_ProcessMessage(uint8_t keyNumber,
                           bool pressed)
{
    for(uint8_t i = 0; i < 9; i++)
    {
        KeypadButton_t *button = &keypadButtons[i];

        if(button->keyNumber == keyNumber)
        {
            /*
             * Detect button press edge
             */

            if(pressed && !button->lastState)
            {
            	if(CHANNEL_FAULTED(button->channel))
            	{
            	    /*
            	     * Reset fault and retry
            	     */

            	    Profet_ResetFault(button->channel);

            	    button->channel->requestedState = true;
            	}
            	else
            	{
            	    /*
            	     * Normal toggle
            	     */

            	    button->channel->requestedState =
            	        !button->channel->requestedState;
            	}
            }

            button->lastState = pressed;
        }
    }
}

//Activate led
 void Keypad_SendLEDCommand(uint8_t keyNumber,uint8_t color,uint8_t mode)
{
    CAN_TxHeaderTypeDef txHeader;

    uint8_t txData[8];

    uint32_t txMailbox;

    txHeader.ExtId =0x18EF0000 |(KEYPAD_CAN_ADDRESS << 8) |PDM_CAN_ADDRESS;
    txHeader.IDE = CAN_ID_EXT;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;
    txData[0] = 0x04;
    txData[1] = 0x1B;
    txData[2] = 0x01;
    txData[3] = keyNumber;
    txData[4] = color;
    txData[5] = mode;
    txData[6] = 0xFF;
    txData[7] = 0xFF;
    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);
    HAL_CAN_AddTxMessage(&hcan1,&txHeader,txData,&txMailbox);
}

 //Keypad Update LED
 void Keypad_UpdateLEDs(void)
 {
     uint8_t newLedState;
     uint8_t newLedMode;

     //Get current system time
     uint32_t now = HAL_GetTick();

     //Process all keypad buttons
     for(uint8_t i = 0; i < 9; i++)
     {
         KeypadButton_t *button = &keypadButtons[i];

         /*
          * =========================================================================
          * Determine LED color and mode
          * =========================================================================
          */

         //Open load fault
         if(button->channel->faultType == FAULT_OPEN_LOAD)
         {
             newLedState = KEYPAD_LED_YELLOW;
             newLedMode = KEYPAD_LED_MODE_BLINK;
         }

         //Overcurrent fault
         else if(button->channel->faultType == FAULT_OVERCURRENT)
         {
             newLedState = KEYPAD_LED_AMBER_ORANGE;
             newLedMode = KEYPAD_LED_MODE_BLINK;
         }

         //Short circuit fault
         else if(button->channel->faultType == FAULT_SHORT_CIRCUIT)
         {
             newLedState = KEYPAD_LED_RED;
             newLedMode = KEYPAD_LED_MODE_BLINK;
         }

         //Thermal fault
         else if(button->channel->faultType == FAULT_THERMAL)
         {
             newLedState = KEYPAD_LED_CYAN;
             newLedMode = KEYPAD_LED_MODE_BLINK;
         }

         //Enabled output
         else if(button->channel->enabled)
         {
             newLedState = KEYPAD_LED_GREEN;
             newLedMode = KEYPAD_LED_MODE_ON;
         }

         //Disabled output
         else
         {
             newLedState = KEYPAD_LED_GREEN;
             newLedMode = KEYPAD_LED_MODE_OFF;
         }

         /*
          * =========================================================================
          * Send LED update if state changed
          * =========================================================================
          */

         if((newLedState != button->lastLedState) ||
            (newLedMode != button->lastLedMode))
         {
             //Send LED command to keypad
             Keypad_SendLEDCommand(
                 button->keyNumber,
                 newLedState,
                 newLedMode);

             //Save current LED state
             button->lastLedState = newLedState;

             //Save current LED mode
             button->lastLedMode = newLedMode;

             //Save time of last update
             button->lastLedUpdate = now;
         }

         /*
          * =========================================================================
          * Periodic LED refresh
          * =========================================================================
          *
          * Resend the current LED state periodically.
          * This prevents the keypad from remaining in the wrong state
          * if a previous CAN frame was lost.
          *
          */

         else if((now - button->lastLedUpdate) >= 100)
         {
             //Resend current LED command
             Keypad_SendLEDCommand(button->keyNumber,newLedState,newLedMode);

             //Update refresh timestamp
             button->lastLedUpdate = now;
         }
     }
 }

 /*
   * =========================================================================
   * Keypad set brightness
   * =========================================================================
   */
void Keypad_SetBrightness(uint8_t percent)
 {
     CAN_TxHeaderTypeDef txHeader;
     uint8_t txData[8];
     uint32_t txMailbox;
     uint8_t brightnessValue;
     /*
      * =========================================================================
      * Limit brightness percentage
      * =========================================================================
      */

     if(percent > 100)
     {
         percent = 100;
     }

     /*
      * =========================================================================
      * Convert 0-100% to 0x00-0x3F
      * =========================================================================
      */

     brightnessValue = (percent * 63) / 100;

     /*
      * =========================================================================
      * Configure CAN frame
      * =========================================================================
      */

     txHeader.ExtId =0x18EF0000 |(KEYPAD_CAN_ADDRESS << 8) |PDM_CAN_ADDRESS;
     txHeader.IDE = CAN_ID_EXT;
     txHeader.RTR = CAN_RTR_DATA;
     txHeader.DLC = 8;
     txData[0] = 0x04;
     txData[1] = 0x1B;
     txData[2] = 0x02;
     //Brightness value
     txData[3] = brightnessValue;
     txData[4] = 0xFF;
     txData[5] = 0xFF;
     txData[6] = 0xFF;
     txData[7] = 0xFF;

     /*
      * =========================================================================
      * Send CAN message
      * =========================================================================
      */

     while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

     HAL_CAN_AddTxMessage( &hcan1,&txHeader,txData,&txMailbox);
 }
