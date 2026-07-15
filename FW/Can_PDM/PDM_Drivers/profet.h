#ifndef PROFET_H
#define PROFET_H

#include "main.h"
#include <stdbool.h>
#include "adc_manager.h"
/*
 * ============================================================================
 * PROFET MODELS
 * ============================================================================
 *
 * Different PROFET devices use different KILIS ratios.
 *
 */

typedef enum
{
    PROFET_BTS7002_1EPP,
    PROFET_BTS7004_1EPZ,
    PROFET_BTS7008_2EPZ

} ProfetModel_t;

#define NUM_Profet_OUTPUTS 9
/*
 * ============================================================================
 * PROFET CHANNEL STRUCTURE
 * ============================================================================
 *
 * Represents one protected high-side output channel.
 *
 */

//Fault structure
typedef enum
{
    FAULT_NONE = 0,
    FAULT_OVERCURRENT,
    FAULT_SHORT_CIRCUIT,
    FAULT_OPEN_LOAD,
    FAULT_THERMAL,
    FAULT_UNDERVOLTAGE

} ProfetFault_t;


#define CHANNEL_FAULTED(channel) ((channel)->faultType != FAULT_NONE)

#define CHANNEL_HARD_FAULT(channel) ((channel)->faultType == FAULT_OVERCURRENT || (channel)->faultType == FAULT_SHORT_CIRCUIT || (channel)->faultType == FAULT_THERMAL)

//Priority structure
typedef enum
{
    PDM_PRIORITY_HIGH     = 1,
    PDM_PRIORITY_MEDIUM   = 2,
    PDM_PRIORITY_LOW      = 3

} PDM_Priority_t;

typedef struct
{
    ProfetModel_t model;    //PROFET model type
    GPIO_TypeDef* enablePort; //Output enable GPIO
    uint16_t enablePin;
    ADC_Channel_t adcChannel; //ADC channel used for current sense
    float kilis; //PROFET current sense ratio
    float rSense; //Current sense resistor value
    float current_mA; //Current
    float maxCurrent_mA; //MaxCurrent Limit
    bool enabled; //1 if is ON
    ProfetFault_t faultType;
    uint8_t priority; //Priority: Low-Mid-High
    bool requestedState; //Request for activation or deactivation
    uint32_t enableTimestamp; //Timestamp for inrush current
    float startupMaxCurrent_mA; //Inrush Current
    uint32_t startupTime_ms; //Startup time
    float shortCircuitCurrent_mA; //Short Circuit current detect
    float openLoadThreshold_mA;
} ProfetChannel_t;

//Extern for define in keypad
extern ProfetChannel_t profet_H1;
extern ProfetChannel_t profet_H2;
extern ProfetChannel_t profet_H3;

extern ProfetChannel_t profet_M1;
extern ProfetChannel_t profet_M2;

extern ProfetChannel_t profet_L1;
extern ProfetChannel_t profet_L2;
extern ProfetChannel_t profet_L3;
extern ProfetChannel_t profet_L4;


/*
 * ============================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================
 */

void Profet_SetState(ProfetChannel_t *channel, bool state); //Enable or disable PROFET output
void Profet_UpdateCurrent(ProfetChannel_t *channel); //Update measured current
float Profet_GetTotalCurrent_mA(void); //Get TotalCurrent
void Profet_CheckFaults(ProfetChannel_t *channel); //Check Fault
void Profet_ResetFault(ProfetChannel_t *channel); //Reset fault

#endif
