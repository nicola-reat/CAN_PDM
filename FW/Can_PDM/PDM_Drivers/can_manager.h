#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include "main.h"
#include <stdbool.h>
#include "pdm_config.h"

//Can rx tx led activity
extern volatile bool canTxActivity;
extern volatile bool canRxActivity;

/*
 * ============================================================================
 * CAN MESSAGE IDs
 * ============================================================================
 */

#if PDM_ID == PDM_1
	#define CAN_ID_PDM_COMMAND          0x200
	#define CAN_ID_PDM_STATUS           0x300
	#define CAN_ID_PDM_H_FAULT_STATUS   0x301
	#define CAN_ID_PDM_M_FAULT_STATUS   0x302
	#define CAN_ID_PDM_L1_FAULT_STATUS  0x303
	#define CAN_ID_PDM_L2_FAULT_STATUS  0x304
	#define CAN_ID_PDM_H_OUTPUTS        0x310
	#define CAN_ID_PDM_M_OUTPUTS        0x311
	#define CAN_ID_PDM_L_OUTPUTS1       0x312
	#define CAN_ID_PDM_L_OUTPUTS2       0x313

#elif PDM_ID == PDM_2

    #define CAN_ID_PDM_COMMAND          0x200
	#define CAN_ID_PDM_STATUS           0x400
	#define CAN_ID_PDM_H_FAULT_STATUS   0x401
	#define CAN_ID_PDM_M_FAULT_STATUS   0x402
	#define CAN_ID_PDM_L1_FAULT_STATUS  0x403
	#define CAN_ID_PDM_L2_FAULT_STATUS  0x404
	#define CAN_ID_PDM_H_OUTPUTS        0x410
	#define CAN_ID_PDM_M_OUTPUTS        0x411
	#define CAN_ID_PDM_L_OUTPUTS1       0x412
	#define CAN_ID_PDM_L_OUTPUTS2       0x413

#endif

/*
 * ============================================================================
 * OUTPUT COMMANDS
 * ============================================================================
 */

typedef enum
{
    PDM_OUTPUT_H1 = 1,
    PDM_OUTPUT_H2,
    PDM_OUTPUT_H3,

    PDM_OUTPUT_M1,
    PDM_OUTPUT_M2,

    PDM_OUTPUT_L1,
    PDM_OUTPUT_L2,
    PDM_OUTPUT_L3,
    PDM_OUTPUT_L4

} PDM_Output_t;


/*
 * ============================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================
 */

//CAN manager initialization
void CAN_Manager_Init(void);

// Process received CAN message
void CAN_ProcessMessage(CAN_RxHeaderTypeDef *rxHeader,uint8_t *rxData);

//Send global status pdm
void CAN_SendPDMStatus(void);

//Send output status
void CAN_SendHighOutputsStatus(void);
void CAN_SendHighOutputsFaults(void);
void CAN_SendMediumOutputsStatus(void);
void CAN_SendMediumOutputsFaults(void);
void CAN_SendLowOutputs1Status(void);
void CAN_SendLow1OutputsFaults(void);
void CAN_SendLowOutputs2Status(void);
void CAN_SendLow2OutputsFaults(void);


#endif
