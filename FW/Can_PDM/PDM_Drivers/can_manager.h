#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include "main.h"
#include <stdbool.h>

//Can rx tx led activity
extern volatile bool canTxActivity;
extern volatile bool canRxActivity;

/*
 * ============================================================================
 * CAN MESSAGE IDs
 * ============================================================================
 */

#define CAN_ID_PDM_COMMAND        				0x200
#define CAN_ID_PDM_STATUS      	  				0x100
#define CAN_ID_PDM_H_FAULT_STATUS      			0x101
#define CAN_ID_PDM_M_FAULT_STATUS      			0x102
#define CAN_ID_PDM_L1_FAULT_STATUS      		0x103
#define CAN_ID_PDM_L2_FAULT_STATUS      		0x104
#define CAN_ID_PDM_H_OUTPUTS      				0x110
#define CAN_ID_PDM_M_OUTPUTS      				0x111
#define CAN_ID_PDM_L_OUTPUTS1     				0x112
#define CAN_ID_PDM_L_OUTPUTS2     				0x113

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
