/*
 * can_ids.h
 *
 *  Created on: Jul 25, 2025
 *      Author: nicola.reat
 */

#ifndef INC_CAN_IDS_H_
#define INC_CAN_IDS_H_

/**
 * @brief  Board CAN ID mapping
 *
 * Each board is assigned a unique BOARD_ID (starting from 1).
 * The board uses a fixed number of CAN IDs for commands and status.
 *
 * This logic maps the CAN ID space for each board:
 *  - TX_BASE_ID = 0x100 + (BOARD_ID - 1) * 0x10  → for outgoing messages (status)
 *  - RX_BASE_ID = 0x200 + (BOARD_ID - 1) * 0x10  → for incoming messages (commands switch)
 *
 * Example with 9 channels per board:
 *  - BOARD_ID 1 → TX: 0x100–0x108, RX: 0x200–0x208
 *  - BOARD_ID 2 → TX: 0x110–0x118, RX: 0x210–0x218
 *  - BOARD_ID 3 → TX: 0x120–0x128, RX: 0x220–0x228
 *
 */

#define BOARD_ID 1

#define TX_BASE_ID (0x100 + ((BOARD_ID - 1) * 0x10))
#define RX_BASE_ID (0x200 + ((BOARD_ID - 1) * 0x10))

// ID Sent From CAN SW PDM
#define CAN_ID_BUTTON_1     (RX_BASE_ID + 0)
#define CAN_ID_BUTTON_2     (RX_BASE_ID + 1)
#define CAN_ID_BUTTON_3     (RX_BASE_ID + 2)
#define CAN_ID_BUTTON_4     (RX_BASE_ID + 3)
#define CAN_ID_BUTTON_5     (RX_BASE_ID + 4)
#define CAN_ID_BUTTON_6     (RX_BASE_ID + 5)
#define CAN_ID_BUTTON_7     (RX_BASE_ID + 6)
#define CAN_ID_BUTTON_8     (RX_BASE_ID + 7)
#define CAN_ID_BUTTON_9     (RX_BASE_ID + 8)

// ID Response From PDM
#define CAN_ID_PROFET_1     (TX_BASE_ID + 0)
#define CAN_ID_PROFET_2     (TX_BASE_ID + 1)
#define CAN_ID_PROFET_3     (TX_BASE_ID + 2)
#define CAN_ID_PROFET_4     (TX_BASE_ID + 3)
#define CAN_ID_PROFET_5     (TX_BASE_ID + 4)
#define CAN_ID_PROFET_6     (TX_BASE_ID + 5)
#define CAN_ID_PROFET_7     (TX_BASE_ID + 6)
#define CAN_ID_PROFET_8     (TX_BASE_ID + 7)
#define CAN_ID_PROFET_9     (TX_BASE_ID + 8)


#endif /* INC_CAN_IDS_H_ */
