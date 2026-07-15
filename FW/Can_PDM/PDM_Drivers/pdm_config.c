#include "pdm_config.h"


//Battery Sense
float Batt_Sense = 0.0f;
//Temperature
float Pdm_Temp = 0.0f;



PDM_State_t pdmState = PDM_STATE_INIT; //Set default state pdm init
//Array pointers for profet channels
ProfetChannel_t* profetChannels[] =
{
    &profet_H1,
    &profet_H2,
    &profet_H3,
    &profet_M1,
    &profet_M2,
    &profet_L1,
    &profet_L2,
    &profet_L3,
    &profet_L4
};

//Set pdfconfig
PDM_Config_t pdmConfig =
{
	//Over Temp
	.overTempWarning_C         = 70.0f, //Warning a 70 gradi

	.overTempShutdownLow_C     = 85.0f, //Inizia a spegnere uscite low priority a 85 gradi
	.overTempLowRecovery_C  = 80.0f, //Hysteresy prima di riaccendere tutto sennò continui ON/OFF

	.overTempShutdownMedium_C  = 95.0f, //Inizia a spegnere uscite medium  priority a 95 gradi
	.overTempMediumRecovery_C = 90.0f, //Hysteresy prima di riaccendere tutto sennò continui ON/OFF

	.overTempCritical_C        = 100.0f, //Spegne tutto
	.overTempCriticalRecovery_C = 95.0f, //Hysteresy prima di riaccendere tutto sennò continui ON/OFF

	//Under voltage
	// Under voltage warning
	.underVoltageWarning_V = 11.8f,

	// Under voltage for Low Priority
	.underVoltageShutdownLow_V = 11.5f,
	.underVoltageLowRecovery_V = 12.0f,

	// Under voltage for Medium Priority
	.underVoltageShutdownMedium_V = 11.2f,
	.underVoltageMediumRecovery_V = 11.8f,

	// Critical undervoltage
	.underVoltageCritical_V = 9.5f,
	.underVoltageCriticalRecovery_V = 10.5f,
};


/*
 * ============================================================================
 * HIGH SIDE OUTPUTS
 * ============================================================================
 *
 * BTS7002-1EPP
 * 3x High current outputs
 *
 */

ProfetChannel_t profet_H1 =
{
    .model = PROFET_BTS7002_1EPP,

    .enablePort = PDM_H_OUT1_EN_GPIO_Port,
    .enablePin = PDM_H_OUT1_EN_Pin,

    .adcChannel = ADC_CH_IS_H1,

    .kilis = 22900.0f,
    .rSense = 1200.0f,
    .current_mA = 0.0f,
	.maxCurrent_mA = 21000.0f,
    .enabled = false,
	.faultType = FAULT_NONE,
	.startupMaxCurrent_mA = 21000.0f,
	.startupTime_ms = 500,
	.shortCircuitCurrent_mA = 21000.0f,
	.openLoadThreshold_mA = 500.0f,
	.priority = PDM_PRIORITY_HIGH
};

ProfetChannel_t profet_H2 =
{
    .model = PROFET_BTS7002_1EPP,

    .enablePort = PDM_H_OUT2_EN_GPIO_Port,
    .enablePin = PDM_H_OUT2_EN_Pin,

    .adcChannel = ADC_CH_IS_H2,

    .kilis = 22900.0f,
    .rSense = 1200.0f,
    .current_mA = 0.0f,
	.maxCurrent_mA = 21000.0f,
    .enabled = false,
	.faultType = FAULT_NONE,
	.startupMaxCurrent_mA = 23000.0f,
	.startupTime_ms = 500,
	.shortCircuitCurrent_mA = 23000.0f,
	.openLoadThreshold_mA = 500.0f,
	.priority = PDM_PRIORITY_HIGH
};

ProfetChannel_t profet_H3 =
{
    .model = PROFET_BTS7002_1EPP,

    .enablePort = PDM_H_OUT3_EN_GPIO_Port,
    .enablePin = PDM_H_OUT3_EN_Pin,

    .adcChannel = ADC_CH_IS_H3,

    .kilis = 22900.0f,
    .rSense = 1200.0f,
    .current_mA = 0.0f,
	.maxCurrent_mA = 21000.0f,
    .enabled = false,
	.faultType = FAULT_NONE,
	.startupMaxCurrent_mA = 21000.0f,
	.startupTime_ms = 500,
	.shortCircuitCurrent_mA = 21000.0f,
	.openLoadThreshold_mA = 500.0f,
	.priority = PDM_PRIORITY_HIGH
};


/*
 * ============================================================================
 * MEDIUM SIDE OUTPUTS
 * ============================================================================
 *
 * BTS7004-1EPZ
 * 2x Medium current outputs
 *
 */

ProfetChannel_t profet_M1 =
{
    .model = PROFET_BTS7004_1EPZ,

    .enablePort = PDM_M_OUT1_EN_GPIO_Port,
    .enablePin = PDM_M_OUT1_EN_Pin,

    .adcChannel = ADC_CH_IS_M1,

    .kilis = 20000.0f,
    .rSense = 1200.0f,

    .current_mA = 0.0f,
	.maxCurrent_mA = 15000.0f,
    .enabled = false,
	.faultType = FAULT_NONE,
	.startupMaxCurrent_mA = 10000.0f,
	.startupTime_ms = 100,
	.shortCircuitCurrent_mA = 15000.0f,
	.openLoadThreshold_mA = 500.0f,
	.priority = PDM_PRIORITY_MEDIUM
};

ProfetChannel_t profet_M2 =
{
    .model = PROFET_BTS7004_1EPZ,

    .enablePort = PDM_M_OUT2_EN_GPIO_Port,
    .enablePin = PDM_M_OUT2_EN_Pin,

    .adcChannel = ADC_CH_IS_M2,

    .kilis = 20000.0f,
    .rSense = 1200.0f,

    .current_mA = 0.0f,
	.maxCurrent_mA = 15000.0f,
    .enabled = false,
	.faultType = FAULT_NONE,
	.startupMaxCurrent_mA = 10000.0f,
	.startupTime_ms = 100,
	.shortCircuitCurrent_mA = 15000.0f,
	.openLoadThreshold_mA = 500.0f,
	.priority = PDM_PRIORITY_MEDIUM
};


/*
 * ============================================================================
 * LOW SIDE OUTPUTS
 * ============================================================================
 *
 * BTS7008-2EPZ
 * Dual channel PROFET devices
 *
 * NOTE:
 * L1/L2 share same current sense
 * L3/L4 share same current sense
 *
 */

ProfetChannel_t profet_L1 =
{
    .model = PROFET_BTS7008_2EPZ,

    .enablePort = PDM_L_OUT1_EN_GPIO_Port,
    .enablePin = PDM_L_OUT1_EN_Pin,

    .adcChannel = ADC_CH_IS_L1_2,

    .kilis = 9500.0f,
    .rSense = 1200.0f,

    .current_mA = 0.0f,
	.maxCurrent_mA = 7500.0f,
    .enabled = false,
	.faultType = FAULT_NONE,
	.startupMaxCurrent_mA = 10000.0f,
	.startupTime_ms = 100,
	.shortCircuitCurrent_mA = 15000.0f,
	.openLoadThreshold_mA = 500.0f,
	.priority = PDM_PRIORITY_LOW
};

ProfetChannel_t profet_L2 =
{
    .model = PROFET_BTS7008_2EPZ,

    .enablePort = PDM_L_OUT2_EN_GPIO_Port,
    .enablePin = PDM_L_OUT2_EN_Pin,

    .adcChannel = ADC_CH_IS_L1_2,

    .kilis = 9500.0f,
    .rSense = 1200.0f,

    .current_mA = 0.0f,
	.maxCurrent_mA = 7500.0f,
    .enabled = false,
	.faultType = FAULT_NONE,
	.startupMaxCurrent_mA = 10000.0f,
	.startupTime_ms = 100,
	.shortCircuitCurrent_mA = 15000.0f,
	.openLoadThreshold_mA = 500.0f,
	.priority = PDM_PRIORITY_LOW
};

ProfetChannel_t profet_L3 =
{
    .model = PROFET_BTS7008_2EPZ,

    .enablePort = PDM_L_OUT3_EN_GPIO_Port,
    .enablePin = PDM_L_OUT3_EN_Pin,

    .adcChannel = ADC_CH_IS_L3_4,

    .kilis = 9500.0f,
    .rSense = 1200.0f,

    .current_mA = 0.0f,
	.maxCurrent_mA = 7500.0f,
    .enabled = false,
	.faultType = FAULT_NONE,
	.startupMaxCurrent_mA = 10000.0f,
	.startupTime_ms = 100,
	.shortCircuitCurrent_mA = 15000.0f,
	.openLoadThreshold_mA = 500.0f,
	.priority = PDM_PRIORITY_LOW
};

ProfetChannel_t profet_L4 =
{
    .model = PROFET_BTS7008_2EPZ,

    .enablePort = PDM_L_OUT4_EN_GPIO_Port,
    .enablePin = PDM_L_OUT4_EN_Pin,

    .adcChannel = ADC_CH_IS_L3_4,

    .kilis = 9500.0f,
    .rSense = 1200.0f,

    .current_mA = 0.0f,
	.maxCurrent_mA = 7500.0f,
    .enabled = false,
	.faultType = FAULT_NONE,
	.startupMaxCurrent_mA = 10000.0f,
	.startupTime_ms = 100,
	.shortCircuitCurrent_mA = 15000.0f,
	.openLoadThreshold_mA = 500.0f,
	.priority = PDM_PRIORITY_LOW
};
