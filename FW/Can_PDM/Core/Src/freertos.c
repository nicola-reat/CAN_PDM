/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc_manager.h"
#include "pdm_config.h"
#include "mcp9808.h"
#include "keypad_manager.h"
#include "can_manager.h"
#include "io_expander.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId acquisitionTaskHandle; //Task acquisition for ADC
osThreadId telemetryTaskHandle; //Task telemetry . Send via can Current etc.
osThreadId powerManagerTaskHandle; //Task power manager for activate / deactivate infineon
osThreadId canLedTaskHandle; //Task for can led
osThreadId outputLedTaskHandle; //Task for o0utputs led
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartAcquisitionTask(void const * argument); //Task for acquisition
void StartTelemetryTask(void const * argument); //Task telemetry . Send via can Current etc.
void StartPowerManagerTask(void const * argument); //Task PowerManager for activate /deactivate infineon
void StartCanLedTask(void const * argument); //Task for blink can led
void StartOutputLedTask(void const * argument); //Task for outputs led
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osThreadDef(acquisitionTask, StartAcquisitionTask, osPriorityAboveNormal, 0, 256);
  acquisitionTaskHandle = osThreadCreate(osThread(acquisitionTask),NULL);

  osThreadDef(telemetryTask,StartTelemetryTask,osPriorityLow,0,256);
  telemetryTaskHandle =osThreadCreate(osThread(telemetryTask),NULL);

  osThreadDef(powerManagerTask,StartPowerManagerTask,osPriorityLow,0,256);
  powerManagerTaskHandle =osThreadCreate(osThread(powerManagerTask),NULL);

  osThreadDef(canLedTask,StartCanLedTask,osPriorityLow,0,256);
  canLedTaskHandle =osThreadCreate(osThread(canLedTask),NULL);

  osThreadDef(outputLedTask,StartOutputLedTask,osPriorityLow,0,256);
  outputLedTaskHandle =osThreadCreate(osThread(outputLedTask),NULL);

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

//Task acquisition
void StartAcquisitionTask(void const * argument)
{
    for(;;)
    {
        // Update battery voltage
        Batt_Sense = ADC_GetBatteryVoltage();

        // Update PDM temperature
        float temperature;

        if (MCP9808_ReadTemperature(&temperature) == HAL_OK)
        {
            Pdm_Temp = temperature;
        }

        // Update all PROFET currents
        for(uint8_t i = 0; i < 9; i++)
        {
            Profet_UpdateCurrent(profetChannels[i]);
            Profet_CheckFaults(profetChannels[i]);
        }

        osDelay(10);
    }
}

//Task telemetry . Send via can Current etc.
void StartTelemetryTask(void const * argument)
{
    for(;;)
    {
        //Global PDM status
        CAN_SendPDMStatus();
        //Outputs telemetry
        CAN_SendHighOutputsStatus();
        CAN_SendHighOutputsFaults();
        CAN_SendMediumOutputsStatus();
        CAN_SendMediumOutputsFaults();
        CAN_SendLowOutputs1Status();
        CAN_SendLow1OutputsFaults();
        CAN_SendLowOutputs2Status();
        CAN_SendLow2OutputsFaults();
        Keypad_UpdateLEDs();
        osDelay(100);
    }
}

//Task Power manager for activate /deactivate profet
void StartPowerManagerTask(void const * argument)
{
	//Temperature
    static bool criticalThermalShutdownActive = false;
    static bool mediumThermalShutdownActive = false;
    static bool lowThermalShutdownActive = false;
    //battery voltage
    static bool lowVoltageShutdownActive = false;
    static bool mediumVoltageShutdownActive = false;
    static bool criticalVoltageShutdownActive = false;
    for(;;)
    {
        //Default state
        pdmState = PDM_STATE_RUN;

        //Undervoltage warning
        if(Batt_Sense < pdmConfig.underVoltageWarning_V)
        {
            pdmState = PDM_STATE_UNDERVOLTAGE;
        }

        //Overtemperature warning
        if(Pdm_Temp > pdmConfig.overTempWarning_C)
        {
            pdmState = PDM_STATE_OVERTEMP;
        }

        //Activate LOW thermal shutdown
        if(Pdm_Temp > pdmConfig.overTempShutdownLow_C)
        {
            lowThermalShutdownActive = true;
        }

        //Recover from LOW thermal shutdown
        if(Pdm_Temp < pdmConfig.overTempLowRecovery_C)
        {
            lowThermalShutdownActive = false;
        }

        //Activate MEDIUM thermal shutdown
        if(Pdm_Temp > pdmConfig.overTempShutdownMedium_C)
        {
            mediumThermalShutdownActive = true;
        }

        //Recover from MEDIUM thermal shutdown
        if(Pdm_Temp < pdmConfig.overTempMediumRecovery_C)
        {
            mediumThermalShutdownActive = false;
        }

        //Activate critical thermal shutdown
        if(Pdm_Temp > pdmConfig.overTempCritical_C)
        {
            criticalThermalShutdownActive = true;
        }

        //Recover from critical thermal shutdown
        if(Pdm_Temp < pdmConfig.overTempCriticalRecovery_C)
        {
            criticalThermalShutdownActive = false;
        }

        //Battery
        //Activate LOW voltage shutdown
        if(Batt_Sense < pdmConfig.underVoltageShutdownLow_V)
        {
            lowVoltageShutdownActive = true;
        }

        //Recover from LOW voltage shutdown
        if(Batt_Sense > pdmConfig.underVoltageLowRecovery_V)
        {
            lowVoltageShutdownActive = false;
        }

        //Activate MEDIUM voltage shutdown
        if(Batt_Sense < pdmConfig.underVoltageShutdownMedium_V)
        {
            mediumVoltageShutdownActive = true;
        }

        //Recover from MEDIUM voltage shutdown
        if(Batt_Sense > pdmConfig.underVoltageMediumRecovery_V)
        {
            mediumVoltageShutdownActive = false;
        }

        //Recover from Critical voltage
        if(Batt_Sense < pdmConfig.underVoltageCritical_V)
        {
            criticalVoltageShutdownActive = true;
        }

        if(Batt_Sense > pdmConfig.underVoltageCriticalRecovery_V)
        {
            criticalVoltageShutdownActive = false;
        }


        //Process all outputs
        for(uint8_t i = 0; i < 9; i++)
        {
            ProfetChannel_t *channel = profetChannels[i];

            //Skip hard-faulted outputs
            if(CHANNEL_HARD_FAULT(channel))
            {
                Profet_SetState(channel, false);
                continue;
            }
            //LOW thermal shutdown
            if(lowThermalShutdownActive &&
               channel->priority == PDM_PRIORITY_LOW)
            {
                Profet_SetState(channel, false);
                continue;
            }

            //MEDIUM thermal shutdown
            if(mediumThermalShutdownActive &&
               channel->priority >= PDM_PRIORITY_MEDIUM)
            {
                Profet_SetState(channel, false);
                continue;
            }

            //CRITICAL thermal shutdown
            if(criticalThermalShutdownActive)
            {
                Profet_SetState(channel, false);
                continue;
            }

            //UnderVoltage protection
            //LOW voltage shutdown
            if(lowVoltageShutdownActive &&
               channel->priority == PDM_PRIORITY_LOW)
            {
                Profet_SetState(channel, false);
                continue;
            }

            //MEDIUM voltage shutdown
            if(mediumVoltageShutdownActive &&
               channel->priority >= PDM_PRIORITY_MEDIUM)
            {
                Profet_SetState(channel, false);
                continue;
            }

            //critical voltage shutdown
            if(criticalVoltageShutdownActive)
            {
                Profet_SetState(channel, false);
                continue;
            }

            //Apply requested state
            Profet_SetState(channel, channel->requestedState);
        }

        osDelay(100);
    }

}

void StartCanLedTask(void const * argument)
{
    for(;;)
    {
        //CAN TX LED
        if(canTxActivity)
        {
            HAL_GPIO_TogglePin(PDM_LED_CAN_TX_GPIO_Port,PDM_LED_CAN_TX_Pin);
            canTxActivity = false; //Reset tx activity
        }

        //CAN RX LED

        if(canRxActivity)
        {
            HAL_GPIO_TogglePin(PDM_LED_CAN_RX_GPIO_Port,PDM_LED_CAN_RX_Pin);
            canRxActivity = false; //Reset rx activity
        }

        osDelay(25);
    }
}

void StartOutputLedTask(void const * argument)
{
  /* Infinite loop */
  for(;;)
  {
	  IOExpander_Update();
	  osDelay(50);
  }
}


/* USER CODE END Application */
