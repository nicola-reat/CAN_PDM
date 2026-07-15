#ifndef PDM_CONFIG_H
#define PDM_CONFIG_H

#include "profet.h"

#define PDM_CAN_ADDRESS      0x30
#define KEYPAD_CAN_ADDRESS   0x21


#define PDM_BATTERY_DIVIDER_RATIO    7.8f
//Global Battery sense config
extern float Batt_Sense;
//Temperature
extern float Pdm_Temp;

//Enum for pdm status
typedef enum
{
    PDM_STATE_INIT = 0,
    PDM_STATE_RUN,
    PDM_STATE_FAULT,
    PDM_STATE_OVERTEMP,
	PDM_STATE_UNDERVOLTAGE,
    PDM_STATE_SLEEP

} PDM_State_t;


//Enum for pdm config
typedef struct
{
	//OverTemperature
    float overTempWarning_C;
    float overTempShutdownLow_C;
    float overTempLowRecovery_C;
    float overTempShutdownMedium_C;
    float overTempMediumRecovery_C;
    float overTempCritical_C;
    float overTempCriticalRecovery_C; //Hysteresys for shutdown

    //OverVoltage
    float underVoltageWarning_V;
    float underVoltageShutdownLow_V;
    float underVoltageLowRecovery_V;
    float underVoltageShutdownMedium_V;
    float underVoltageMediumRecovery_V;
    float underVoltageCritical_V; //Critical voltage turn off
    float underVoltageCriticalRecovery_V; //Critical voltage turn off

} PDM_Config_t;



extern PDM_State_t pdmState; //pdm state
extern ProfetChannel_t* profetChannels[]; //Array pointers
extern PDM_Config_t pdmConfig;
/*
 * ============================================================================
 * GLOBAL PROFET CHANNELS
 * ============================================================================
 */

extern ProfetChannel_t profet_H1;
extern ProfetChannel_t profet_H2;
extern ProfetChannel_t profet_H3;

extern ProfetChannel_t profet_M1;
extern ProfetChannel_t profet_M2;

extern ProfetChannel_t profet_L1;
extern ProfetChannel_t profet_L2;
extern ProfetChannel_t profet_L3;
extern ProfetChannel_t profet_L4;





#endif
