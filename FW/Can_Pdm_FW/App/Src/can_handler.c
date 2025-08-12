/*
 * can_handler.c
 *
 *  Created on: Jul 28, 2025
 *      Author: Reatn
 */


#include "can_handler.h"
#include "cmsis_os.h"
#include "string.h"
#include "settings.h"
#include "gpio.h"
#include "profet.h"

volatile bool can_rx_flag = false; //For blink led


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

    CAN_RxHeaderTypeDef rxh;
    uint8_t rxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxh, rxData) != HAL_OK)
        return;
    can_rx_flag = true;  // Set flag



    uint16_t id = rxh.StdId;
    bool state = (rxData[0] == 1);

    switch (id) {
        case CAN_ID_BUTTON_1:
            Profet_SetState(&profet_Hout1, state);
            break;
        case CAN_ID_BUTTON_2:
            Profet_SetState(&profet_Hout2, state);
            break;
        case CAN_ID_BUTTON_3:
            Profet_SetState(&profet_Hout3, state);
            break;
        case CAN_ID_BUTTON_4:
            Profet_SetState(&profet_Mout1, state);
            break;
        case CAN_ID_BUTTON_5:
            Profet_SetState(&profet_Mout2, state);
            break;
        case CAN_ID_BUTTON_6:
            Profet_SetState(&profet_Lout1, state);
            break;
        case CAN_ID_BUTTON_7:
            Profet_SetState(&profet_Lout2, state);
            break;
        case CAN_ID_BUTTON_8:
            Profet_SetState(&profet_Lout3, state);
            break;
        case CAN_ID_BUTTON_9:
            Profet_SetState(&profet_Lout4, state);
            break;
        default:
            break;
    }
}





