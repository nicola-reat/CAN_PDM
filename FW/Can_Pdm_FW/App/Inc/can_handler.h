/*
 * can_handler.h
 *
 *  Created on: Jul 28, 2025
 *      Author: Reatn
 */

#ifndef INC_CAN_HANDLER_H_
#define INC_CAN_HANDLER_H_

#include "main.h"
#include "can.h"
#include <stdbool.h>

extern volatile bool can_rx_flag;

typedef struct {
    uint32_t id;         // ID Can message
    uint8_t data[8];     // Can message data
    uint8_t len;         // Lenght can message
} CanMessage_t;


#endif /* INC_CAN_HANDLER_H_ */
