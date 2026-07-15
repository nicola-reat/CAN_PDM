#include "can_manager.h"
#include "can.h"
#include "profet.h"
#include "pdm_config.h"
#include "keypad_manager.h"
#include "gpio.h"
//For pdm heartbeat
static uint8_t aliveCounter = 0;
//Led tx / rx
volatile bool canTxActivity = false;
volatile bool canRxActivity = false;
/*
 * ============================================================================
 * CAN RX VARIABLES
 * ============================================================================
 */

CAN_RxHeaderTypeDef canRxHeader;
uint8_t canRxData[8];


/*
 * ============================================================================
 * CAN_Manager_Init
 * ============================================================================
 */

void CAN_Manager_Init(void)
{
    CAN_FilterTypeDef canFilter;

    /*
     * =========================================================================
     * FILTER CONFIGURATION
     * =========================================================================
     */
    canFilter.FilterBank = 0; //Use filter bank 0
    canFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0; //Assign filter to FIFO0
    canFilter.FilterScale = CAN_FILTERSCALE_32BIT; //32-bit filter mode
    canFilter.FilterMode = CAN_FILTERMODE_IDMASK; // ID MASK MODE
    canFilter.FilterIdHigh = 0;
    canFilter.FilterIdLow = 0;
    canFilter.FilterMaskIdHigh = 0;
    canFilter.FilterMaskIdLow = 0;
    canFilter.FilterActivation = ENABLE; //Enable filter
    HAL_CAN_ConfigFilter(&hcan1, &canFilter); //Configure CAN filter hardware
    HAL_CAN_Start(&hcan1); //Start CAN
    HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING); //Enable RX FIFO0 interrupt - Interrupt triggers when new CAN message arrives
}


/*
 * ============================================================================
 * CAN RX CALLBACK
 * ============================================================================
 */

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	//Rx led
	canRxActivity = true;
	//Read received CAN message
    HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&canRxHeader,canRxData);
    CAN_ProcessMessage(&canRxHeader, canRxData); //Process CAN message
}


/*
 * ============================================================================
 * CAN_ProcessMessage
 * ============================================================================
 */

void CAN_ProcessMessage(CAN_RxHeaderTypeDef *rxHeader,
                        uint8_t *rxData)
{
    /*
     * ============================================================
     * EXTENDED CAN FRAMES
     * ============================================================
     */

    if(rxHeader->IDE == CAN_ID_EXT)
    {
        if(rxHeader->ExtId == 0x18EFFF21)
        {
            uint8_t keyNumber = rxData[3];
            bool pressed = rxData[4];

            Keypad_ProcessMessage(keyNumber, pressed);
        }

        return;
    }

    /*
     * ============================================================
     * STANDARD CAN FRAMES
     * ============================================================
     */

    if(rxHeader->StdId == CAN_ID_PDM_COMMAND)
    {
        PDM_Output_t output;
        bool state;

        output = rxData[0];
        state = rxData[1];

        switch(output)
        {
            case PDM_OUTPUT_H1:
                profet_H1.requestedState = state;
                break;

            case PDM_OUTPUT_H2:
                profet_H2.requestedState = state;
                break;

            case PDM_OUTPUT_H3:
                profet_H3.requestedState = state;
                break;

            case PDM_OUTPUT_M1:
                profet_M1.requestedState = state;
                break;

            case PDM_OUTPUT_M2:
                profet_M2.requestedState = state;
                break;

            case PDM_OUTPUT_L1:
                profet_L1.requestedState = state;
                break;

            case PDM_OUTPUT_L2:
                profet_L2.requestedState = state;
                break;

            case PDM_OUTPUT_L3:
                profet_L3.requestedState = state;
                break;

            case PDM_OUTPUT_L4:
                profet_L4.requestedState = state;
                break;

            default:
                break;
        }
    }
}

//Send PDM status
//Byte 0	Alive heartbeat
//Byte 1	PDM state
//Byte 2	Total current LSB
//Byte 3	Total current MSB
//Byte 4	Battery voltage LSB
//Byte 5	Battery voltage MSB
//Byte 6	Temperature LSB
//Byte 7	Temperature MSB

void CAN_SendPDMStatus(){
	CAN_TxHeaderTypeDef txHeader;
	uint8_t txData[8];
	uint32_t txMailbox;
	txHeader.StdId = CAN_ID_PDM_STATUS; //Standard 11-bit ID
	txHeader.IDE = CAN_ID_STD; //Standard frame (not extended)
	txHeader.RTR = CAN_RTR_DATA; //Data frame
	txHeader.DLC = 8; //N of bytes


	//Alive Heartbeat
	txData[0] = aliveCounter++;
	//Pdm Status
	txData[1] = pdmState;
	//Total current
	float totalCurrent_mA =Profet_GetTotalCurrent_mA();
	uint16_t totalCurrent =(uint16_t)((totalCurrent_mA / 1000.0f) * 10.0f);
	txData[2] = totalCurrent & 0xFF;
	txData[3] = (totalCurrent >> 8) & 0xFF;
	//Battery Voltage
	uint16_t batteryVoltage =(uint16_t)(Batt_Sense * 100.0f); //Battery voltage x100
    txData[4] = batteryVoltage & 0xFF;
    txData[5] = (batteryVoltage >> 8) & 0xFF;

    //Temperature
    uint16_t PdmTemperature = (uint16_t)(Pdm_Temp * 100.0f); //Temperature x100
    txData[6] = PdmTemperature & 0xFF;
    txData[7] = (PdmTemperature >> 8) & 0xFF;
    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0); //wair for free TX mailbox
    //Send message
    HAL_CAN_AddTxMessage(&hcan1,&txHeader,txData,&txMailbox);
    //Led TX
    canTxActivity = true;

}

//Send via can high output status (current, status fault)
void CAN_SendHighOutputsStatus(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8];
    uint32_t txMailbox;
    txHeader.StdId = CAN_ID_PDM_H_OUTPUTS;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;

    /*
     * =========================================================================
     * Status Byte
     * =========================================================================
     *
     * Bit0 = H1 enabled
     * Bit1 = H2 enabled
     * Bit2 = H3 enabled
     *
     * Bit3 = H1 fault
     * Bit4 = H2 fault
     * Bit5 = H3 fault
     *
     */

    uint8_t statusByte = 0;

    //Enabled bits
    if(profet_H1.enabled)
        statusByte |= (1 << 0);

    if(profet_H2.enabled)
        statusByte |= (1 << 1);

    if(profet_H3.enabled)
        statusByte |= (1 << 2);

    //Fault bits
    if(CHANNEL_FAULTED(&profet_H1))
        statusByte |= (1 << 3);

    if(CHANNEL_FAULTED(&profet_H2))
        statusByte |= (1 << 4);

    if(CHANNEL_FAULTED(&profet_H3))
        statusByte |= (1 << 5);

    txData[0] = statusByte;

    /*
     * =========================================================================
     * Currents
     * =========================================================================
     *
     * Current format:
     * A x10
     *
     */

    uint16_t h1Current =
        (uint16_t)((profet_H1.current_mA / 1000.0f) * 10.0f);
    uint16_t h2Current =
        (uint16_t)((profet_H2.current_mA / 1000.0f) * 10.0f);
    uint16_t h3Current =
        (uint16_t)((profet_H3.current_mA / 1000.0f) * 10.0f);
    //H1 current
    txData[1] = h1Current & 0xFF;
    txData[2] = (h1Current >> 8) & 0xFF;
    //H2 current
    txData[3] = h2Current & 0xFF;
    txData[4] = (h2Current >> 8) & 0xFF;
    //H3 current
    txData[5] = h3Current & 0xFF;
    txData[6] = (h3Current >> 8) & 0xFF;
    //Reserved
    txData[7] = 0;
    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0); //wair for free TX mailbox
    //Send CAN message
    HAL_CAN_AddTxMessage(&hcan1,&txHeader,txData,&txMailbox);
    //Led TX
    canTxActivity = true;
}

////Send via can high output fault status (type of fault)
void CAN_SendHighOutputsFaults(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8] = {0};
    uint32_t txMailbox;

    txHeader.StdId = CAN_ID_PDM_H_FAULT_STATUS;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;

    //Fault types
    txData[0] = profet_H1.faultType;
    txData[1] = profet_H2.faultType;
    txData[2] = profet_H3.faultType;

    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

    HAL_CAN_AddTxMessage(&hcan1,&txHeader,txData,&txMailbox);
    //Led TX
    canTxActivity = true;
}

//Send via can medium output status (current, status fault)
void CAN_SendMediumOutputsStatus(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8];
    uint32_t txMailbox;
    txHeader.StdId = CAN_ID_PDM_M_OUTPUTS;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;

    /*
     * Status Byte
     *
     * Bit0 = M1 enabled
     * Bit1 = M2 enabled
     *
     * Bit2 = M1 fault
     * Bit3 = M2 fault
     */

    uint8_t statusByte = 0;

    //Enabled bits
    if(profet_M1.enabled)
        statusByte |= (1 << 0);

    if(profet_M2.enabled)
        statusByte |= (1 << 1);

    //Fault bits
    if(CHANNEL_FAULTED(&profet_M1))
        statusByte |= (1 << 2);

    if(CHANNEL_FAULTED(&profet_M2))
        statusByte |= (1 << 3);

    txData[0] = statusByte;

    /*
     * Currents
     * Format = A x10
     */

    uint16_t m1Current =
        (uint16_t)((profet_M1.current_mA / 1000.0f) * 10.0f);

    uint16_t m2Current =
        (uint16_t)((profet_M2.current_mA / 1000.0f) * 10.0f);

    //M1 current
    txData[1] = m1Current & 0xFF;
    txData[2] = (m1Current >> 8) & 0xFF;
    //M2 current
    txData[3] = m2Current & 0xFF;
    txData[4] = (m2Current >> 8) & 0xFF;
    //Reserved
    txData[5] = 0;
    txData[6] = 0;
    txData[7] = 0;
    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0); //wair for free TX mailbox
    //Send message
    HAL_CAN_AddTxMessage(&hcan1,&txHeader,txData,&txMailbox);
    //Led TX
    canTxActivity = true;
}

////Send via can medium output fault status (type of fault)
void CAN_SendMediumOutputsFaults(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8] = {0};
    uint32_t txMailbox;

    txHeader.StdId = CAN_ID_PDM_M_FAULT_STATUS;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;

    //Fault types
    txData[0] = profet_M1.faultType;
    txData[1] = profet_M2.faultType;

    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

    HAL_CAN_AddTxMessage(&hcan1,&txHeader,txData,&txMailbox);
    //Led TX
    canTxActivity = true;
}

//Send via can low1 output status (current, status fault)
void CAN_SendLowOutputs1Status(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8];
    uint32_t txMailbox;
    txHeader.StdId = CAN_ID_PDM_L_OUTPUTS1;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;

    /*
     * Status Byte
     *
     * Bit0 = L1 enabled
     * Bit1 = L2 enabled
     *
     * Bit2 = L1 fault
     * Bit3 = L2 fault
     */

    uint8_t statusByte = 0;

    //Enabled bits
    if(profet_L1.enabled)
        statusByte |= (1 << 0);

    if(profet_L2.enabled)
        statusByte |= (1 << 1);

    //Fault bits
    if(CHANNEL_FAULTED(&profet_L1))
        statusByte |= (1 << 2);

    if(CHANNEL_FAULTED(&profet_L2))
        statusByte |= (1 << 3);

    txData[0] = statusByte;

    /*
     * Currents
     * Format = A x10
     */

    uint16_t l1Current =
        (uint16_t)((profet_L1.current_mA / 1000.0f) * 10.0f);

    uint16_t l2Current =
        (uint16_t)((profet_L2.current_mA / 1000.0f) * 10.0f);

    //L1 current
    txData[1] = l1Current & 0xFF;
    txData[2] = (l1Current >> 8) & 0xFF;
    //L2 current
    txData[3] = l2Current & 0xFF;
    txData[4] = (l2Current >> 8) & 0xFF;
    //Reserved
    txData[5] = 0;
    txData[6] = 0;
    txData[7] = 0;
    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0); //wair for free TX mailbox
    //Send message
    HAL_CAN_AddTxMessage(&hcan1,&txHeader,txData,&txMailbox);
    //Led TX
    canTxActivity = true;
}

////Send via can low1 output fault status (type of fault)
void CAN_SendLow1OutputsFaults(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8] = {0};
    uint32_t txMailbox;

    txHeader.StdId = CAN_ID_PDM_L1_FAULT_STATUS;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;

    //Fault types
    txData[0] = profet_L1.faultType;
    txData[1] = profet_L2.faultType;

    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

    HAL_CAN_AddTxMessage(&hcan1,&txHeader,txData,&txMailbox);
    //Led TX
    canTxActivity = true;
}

//Send via can low2 output status (current, status fault)
void CAN_SendLowOutputs2Status(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8];
    uint32_t txMailbox;
    txHeader.StdId = CAN_ID_PDM_L_OUTPUTS2;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;

    /*
     * Status Byte
     *
     * Bit0 = L3 enabled
     * Bit1 = L4 enabled
     *
     * Bit2 = L3 fault
     * Bit3 = L4 fault
     */

    uint8_t statusByte = 0;

    //Enabled bits
    if(profet_L3.enabled)
        statusByte |= (1 << 0);

    if(profet_L4.enabled)
        statusByte |= (1 << 1);

    //Fault bits
    if(CHANNEL_FAULTED(&profet_L3))
        statusByte |= (1 << 2);

    if(CHANNEL_FAULTED(&profet_L4))
        statusByte |= (1 << 3);

    txData[0] = statusByte;

    /*
     * Currents
     * Format = A x10
     */

    uint16_t l3Current =
        (uint16_t)((profet_L3.current_mA / 1000.0f) * 10.0f);

    uint16_t l4Current =
        (uint16_t)((profet_L4.current_mA / 1000.0f) * 10.0f);

    //L3 current
    txData[1] = l3Current & 0xFF;
    txData[2] = (l3Current >> 8) & 0xFF;

    //L4 current
    txData[3] = l4Current & 0xFF;
    txData[4] = (l4Current >> 8) & 0xFF;

    //Reserved
    txData[5] = 0;
    txData[6] = 0;
    txData[7] = 0;
    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0); //wair for free TX mailbox
    //Send message
    HAL_CAN_AddTxMessage(&hcan1,&txHeader,txData,&txMailbox);
    //Led TX
    canTxActivity = true;
}

////Send via can low2 output fault status (type of fault)
void CAN_SendLow2OutputsFaults(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8] = {0};
    uint32_t txMailbox;

    txHeader.StdId = CAN_ID_PDM_L2_FAULT_STATUS;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;

    //Fault types
    txData[0] = profet_L3.faultType;
    txData[1] = profet_L4.faultType;

    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

    HAL_CAN_AddTxMessage(&hcan1,&txHeader,txData,&txMailbox);
    //Led TX
    canTxActivity = true;
}
