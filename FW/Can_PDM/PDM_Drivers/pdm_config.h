#ifndef PDM_CONFIG_H
#define PDM_CONFIG_H



#define KEYPAD_CAN_ADDRESS   0x21

#define PDM_1 1
#define PDM_2 2

#define PDM_ID PDM_1 //Change pdm here

#if PDM_ID == PDM_1
#define PDM_CAN_ADDRESS 0x30
#elif PDM_ID == PDM_2
#define PDM_CAN_ADDRESS 0x31
#else
#error "Invalid PDM_ID"
#endif

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
extern PDM_Config_t pdmConfig;




#endif
