/*
 * rrrc_worklogic.c
 *
 * Created: 2/14/2019 11:38:22 AM
 *  Author: User
 */ 

#include "rrrc_hal.h"
#include "rrrc_worklogic.h"
#include "rrrc_i2c_protocol.h"
#include "rrrc_sensors.h"
#include "rrrc_motors.h"
#include "rrrc_indication.h"
#include "rrrc_sysmon.h"

#include "jscope/jscope.h"

#include "components/ADC/adc.h"
#include "components/BatteryCharger/BatteryCharger.h"
#include "components/InternalTemperatureSensor/InternalTemperatureSensor.h"
#include "components/LEDController/LEDController.h"
#include "components/BluetoothIndicator/BluetoothIndicator.h"
#include "components/BrainStatusIndicator/BrainStatusIndicator.h"
#include "components/BatteryCalculator/BatteryCalculator.h"
#include "components/BatteryIndicator/BatteryIndicator.h"
#include "components/RingLedDisplay/RingLedDisplay.h"
#include "components/MasterCommunicationInterface/MasterCommunicationInterface.h"
#include "components/MasterCommunication/MasterCommunication.h"
#include "components/CommunicationObserver/CommunicationObserver.h"
#include "components/MasterStatusObserver/MasterStatusObserver.h"
#include "components/BluetoothStatusObserver/BluetoothStatusObserver.h"
#include "components/VersionProvider/VersionProvider.h"
#include "components/BatteryStatusProvider/BatteryStatusProvider.h"
#include "components/MotorPortHandler/MotorPortHandler.h"
#include "components/SensorPortHandler/SensorPortHandler.h"

#include <math.h>

static TaskHandle_t xRRRC_Main_xTask;

static BatteryCalculator_Context_t mainBattery;
static BatteryCalculator_Context_t motorBattery;

static BatteryIndicator_Context_t mainBatteryIndicator;
static BatteryIndicator_Context_t motorBatteryIndicator;

static bool mainBatteryDetected;
static bool motorBatteryDetected;
static float mainBatteryVoltage;
static float motorBatteryVoltage;
static uint8_t mainBatteryPercentage;
static uint8_t motorBatteryPercentage;

/* TODO these are extern */
extern hw_motor_port_t motor_ports[MOTOR_PORT_AMOUNT];
extern hw_sensor_port_t sensor_ports[SENSOR_PORT_AMOUNT];

static MasterStatus_t masterStatus;
static bool isBleConnected;

Comm_Status_t PingMessageHandler_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    return Comm_Status_Ok;
}

static const Comm_CommandHandler_t communicationHandlers[] = 
{
    /* other commands */
    [0x00u] = { .Start = &PingMessageHandler_Start, .GetResult = NULL, .Cancel = NULL },
    [0x01u] = { .Start = &VersionProvider_GetHardwareVersion_Start, .GetResult = NULL, .Cancel = NULL },
    [0x02u] = { .Start = &VersionProvider_GetFirmwareVersion_Start, .GetResult = NULL, .Cancel = NULL },
    [0x03u] = { .Start = &BatteryStatusProvider_Start, .GetResult = NULL, .Cancel = NULL },
    [0x04u] = { .Start = &MasterStatusObserver_SetMasterStatus_Start, .GetResult = NULL, .Cancel = NULL },
    [0x05u] = { .Start = &BluetoothStatusObserver_SetBluetoothStatus_Start, .GetResult = NULL, .Cancel = NULL },
    
    /* motor commands */
    [0x10u] = { .Start = &MotorPortHandler_GetMotorPortAmount_Start, .GetResult = NULL, .Cancel = NULL },
    [0x11u] = { .Start = &MotorPortHandler_GetMotorPortTypes_Start, .GetResult = NULL, .Cancel = NULL },

    /* sensor commands */
    [0x20u] = { .Start = &SensorPortHandler_GetSensorPortAmount_Start, .GetResult = NULL, .Cancel = NULL },
    [0x21u] = { .Start = &SensorPortHandler_GetSensorPortTypes_Start, .GetResult = NULL, .Cancel = NULL },

    /* led ring commands */
};

static MasterCommunicationInterface_Config_t communicationConfig = 
{
    .rxTimeout = 100u, /* ms */
};

void RRRC_ProcessLogic_xTask(void* user_data);

//*********************************************************************************************
static void SensorsPinsInit()
{
    //led pins
    for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
    {
        gpio_set_pin_pull_mode(sensor_ports[idx].led0_gpio, GPIO_PULL_UP);
        gpio_set_pin_function(sensor_ports[idx].led0_gpio, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(sensor_ports[idx].led0_gpio, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(sensor_ports[idx].led0_gpio, false);

        gpio_set_pin_pull_mode(sensor_ports[idx].led1_gpio, GPIO_PULL_UP);
        gpio_set_pin_function(sensor_ports[idx].led1_gpio, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(sensor_ports[idx].led1_gpio, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(sensor_ports[idx].led1_gpio, false);
    }

    //gpio0 - in & extint
    for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
    {
        gpio_set_pin_pull_mode(sensor_ports[idx].gpio0_num, GPIO_PULL_OFF);
        gpio_set_pin_direction(sensor_ports[idx].gpio0_num, GPIO_DIRECTION_IN);
        gpio_set_pin_function(sensor_ports[idx].gpio0_num, GPIO_PIN_FUNCTION_A);
    }

    //gpio1 - out
    for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
    {
        gpio_set_pin_pull_mode(sensor_ports[idx].gpio1_num, GPIO_PULL_OFF);
        gpio_set_pin_direction(sensor_ports[idx].gpio1_num, GPIO_DIRECTION_OUT);
        gpio_set_pin_function(sensor_ports[idx].gpio1_num, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_level(sensor_ports[idx].gpio1_num, false);
    }

    //adc pins
    for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
    {
        gpio_set_pin_direction(sensor_ports[idx].adc_gpio, GPIO_DIRECTION_OFF);
        gpio_set_pin_function(sensor_ports[idx].adc_gpio, GPIO_PIN_FUNCTION_B);
    }

    // i2c pins - TODO now this pins init in driver_init()
//     for  (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
//     {
//         gpio_set_pin_pull_mode(sensor_ports[idx].i2c_gpio0, GPIO_PULL_OFF);
//         gpio_set_pin_direction(sensor_ports[idx].i2c_gpio0, GPIO_DIRECTION_OFF);
//         gpio_set_pin_function(sensor_ports[idx].i2c_gpio0, GPIO_PIN_FUNCTION_C);
//         gpio_set_pin_pull_mode(sensor_ports[idx].i2c_gpio1, GPIO_PULL_OFF);
//         gpio_set_pin_direction(sensor_ports[idx].i2c_gpio1, GPIO_DIRECTION_OFF);
//         gpio_set_pin_function(sensor_ports[idx].i2c_gpio1, GPIO_PIN_FUNCTION_C);
//     }

    for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++) 
    {
        gpio_set_pin_pull_mode(sensor_ports[idx].vccio_pin, GPIO_PULL_UP);
        gpio_set_pin_function(sensor_ports[idx].vccio_pin, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(sensor_ports[idx].vccio_pin, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(sensor_ports[idx].vccio_pin, false);//VCCIO_3V3
    }
}

//*********************************************************************************************
static void MotorsPinsInit()
{
    //led pins
    for (int idx=0; idx<ARRAY_SIZE(motor_ports); idx++)
    {
        gpio_set_pin_pull_mode(motor_ports[idx].led0_gpio, GPIO_PULL_UP);
        gpio_set_pin_function(motor_ports[idx].led0_gpio, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(motor_ports[idx].led0_gpio, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(motor_ports[idx].led0_gpio, false);

        gpio_set_pin_pull_mode(motor_ports[idx].led1_gpio, GPIO_PULL_UP);
        gpio_set_pin_function(motor_ports[idx].led1_gpio, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(motor_ports[idx].led1_gpio, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(motor_ports[idx].led1_gpio, false);
    }

    for (int idx=0; idx<ARRAY_SIZE(motor_ports); idx++)
    {
        tb6612fng_one_time_init(&motor_ports[idx]);

        // PWM
        gpio_set_pin_direction(motor_ports[idx].pwm_pin, GPIO_DIRECTION_OFF);
        gpio_set_pin_function(motor_ports[idx].pwm_pin, GPIO_PIN_FUNCTION_F);
    }

    uint8_t moto_stbypins[] = {M12STBY, M34STBY, M56STBY};
    for (int idx=0; idx<ARRAY_SIZE(moto_stbypins); idx++) // chips always ON
    {
        gpio_set_pin_pull_mode(moto_stbypins[idx], GPIO_PULL_UP);
        gpio_set_pin_function(moto_stbypins[idx], GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(moto_stbypins[idx], GPIO_DIRECTION_OUT);
        gpio_set_pin_level(moto_stbypins[idx], true);
    }

    gpio_set_pin_pull_mode(MOTPWEN, GPIO_PULL_UP);
    gpio_set_pin_function(MOTPWEN, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(MOTPWEN, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(MOTPWEN, true);
}

//*********************************************************************************************
void SystemMonitorPinsInit(void)
{
    gpio_set_pin_direction(SM_MOT_CURRENT_FAULT, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SM_MOT_CURRENT_FAULT, GPIO_PULL_UP);
    gpio_set_pin_function(SM_MOT_CURRENT_FAULT, GPIO_PIN_FUNCTION_OFF);

    //adc pins
    gpio_set_pin_direction(SM_MOT_VOLTAGE, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(SM_MOT_VOLTAGE, GPIO_PIN_FUNCTION_B);

    gpio_set_pin_direction(SM_MOT_CURRENT, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(SM_MOT_CURRENT, GPIO_PIN_FUNCTION_B);

    gpio_set_pin_direction(SM_BAT_VOLTAGE, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(SM_BAT_VOLTAGE, GPIO_PIN_FUNCTION_B);
}

//*********************************************************************************************
int32_t RRRC_Init(void)
{
    int32_t result = ERR_NONE;

    jscope_init();

    MotorsPinsInit();
    SensorsPinsInit();
    SystemMonitorPinsInit();

    for (uint32_t idx=0; idx<SENSOR_PORT_AMOUNT; idx++ )
    {
        result = SensorPortInit(idx);
        if (result != ERR_NONE)
            return result;
    }

    for (uint32_t idx=0; idx<MOTOR_PORT_AMOUNT; idx++ )
    {
        result = MotorPortInit(idx);
        if (result != ERR_NONE)
            return result;
    }

//    result = RRRC_Communication_Init();

    if (pdPASS != xTaskCreate(RRRC_ProcessLogic_xTask, "RRRC_Main", 1024u, NULL, taskPriority_Main, &xRRRC_Main_xTask))
    {
        return ERR_FAILURE;
    }

//     SensorPortSetType(0,SENSOR_HC_SR05);
//     SensorPortSetType(1,SENSOR_ANALOG_SWITCH);
//    IndicationSetRingType(RING_LED_PREDEF_4);

    return ERR_NONE;
}

int32_t RRRC_DeInit(void)
{
//    RRRC_Communication_DeInit();
    
    vTaskDelete(xRRRC_Main_xTask);

    for (uint32_t idx=0; idx<SENSOR_PORT_AMOUNT; idx++ )
        SensorPortDeInit(idx);
    for (uint32_t idx=0; idx<MOTOR_PORT_AMOUNT; idx++ )
        MotorPortDeInit(idx);

    return ERR_NONE;
}

static void ProcessTasks_10ms(void)
{
    ADC_Run_Update();
    BatteryCharger_Run_Update();
}

static void ProcessTasks_20ms(void)
{
    /* TODO: Different ports may be handled with a time offset from each other to decrease jitter */
    for (uint32_t i = 0u; i < MOTOR_PORT_AMOUNT; i++)
    {
        /* Critical section (temporarily) necessary because motor ports can be configured in a different thread */
        taskENTER_CRITICAL();
        MotorPort_Update(i);
        taskEXIT_CRITICAL();
    }

    RingLedDisplay_Run_Update();
    LEDController_Run_Update();
}

static void ProcessTasks_100ms(void)
{
    BatteryCalculator_Run_Update(&mainBattery);
    BatteryCalculator_Run_Update(&motorBattery);

    BatteryIndicator_Run_Update(&mainBatteryIndicator);
    BatteryIndicator_Run_Update(&motorBatteryIndicator);

    BluetoothIndicator_Run_Update();
    BrainStatusIndicator_Run_Update();
}

//*********************************************************************************************
void RRRC_ProcessLogic_xTask(void* user)
{
    ADC_Run_OnInit();
    BatteryCharger_Run_OnInit();
    LEDController_Run_OnInit();
    BluetoothIndicator_Run_OnInit();
    BrainStatusIndicator_Run_OnInit();
    
    MasterStatusObserver_Run_OnInit();
    BluetoothStatusObserver_Run_OnInit();

    MasterCommunication_Run_OnInit(&communicationHandlers[0], ARRAY_SIZE(communicationHandlers));

    MasterCommunication_Run_GetDefaultResponse(&communicationConfig.defaultResponseBuffer, &communicationConfig.defaultResponseLength);
    MasterCommunication_Run_GetLongRxErrorResponse(&communicationConfig.longRxErrorResponseBuffer, &communicationConfig.longRxErrorResponseLength);

    MasterCommunicationInterface_Run_OnInit(&communicationConfig);
    
    /* 1 cell LiPoly */
    mainBattery.detectionVoltage = 2000.0f;
    mainBattery.minVoltage = 3100.0f;
    mainBattery.maxVoltage = 4100.0f;

    BatteryCalculator_Run_OnInit(&mainBattery);
    BatteryIndicator_Run_OnInit(&mainBatteryIndicator);
    
    /* 6xAAA rechargeable */
    motorBattery.detectionVoltage = 4000.0f;
    motorBattery.minVoltage = 5400.0f;
    motorBattery.maxVoltage = 7000.0f;

    BatteryCalculator_Run_OnInit(&motorBattery);
    BatteryIndicator_Run_OnInit(&motorBatteryIndicator);

    RingLedDisplay_Run_OnInit();
    CommunicationObserver_Run_OnInit();

    BatteryCharger_Run_EnableFastCharge();

    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (uint8_t cycleCounter = 0u;;)
    {
        ProcessTasks_10ms();
        
        if (cycleCounter % 2 == 0)
        {
            ProcessTasks_20ms();
        }
        if (cycleCounter == 9u)
        {
            ProcessTasks_100ms();
            cycleCounter = 0u;
        }
        else
        {
            cycleCounter++;
        }

        vTaskDelayUntil(&xLastWakeTime, rtos_ms_to_ticks(10));
    }
}

#define SYSMON_ADC_MOTOR_VOLTAGE   ADC_BUFFER_ADC1_04
#define SYSMON_ADC_MOTOR_CURRENT   ADC_BUFFER_ADC1_10
#define SYSMON_ADC_BATTERY_VOLTAGE ADC_BUFFER_ADC1_11
#define SYSMON_ADC_TEMPERATURE_P   ADC_BUFFER_ADC1_28
#define SYSMON_ADC_TEMPERATURE_C   ADC_BUFFER_ADC1_29

void ADC_Write_RawSamples_ADC0(uint16_t samples[4])
{
    for (uint32_t i = 0u; i < 4u; i++)
    {
        SensorPort_Adc_Update(i, samples[i]);
    }
}

void ADC_Write_Samples_ADC1(float samples[5])
{
    rrrc_sysmon_t sysmon;
    sysmon.systicks = xTaskGetTickCount();
    sysmon.motor_voltage   = (uint32_t) lroundf(samples[SYSMON_ADC_MOTOR_VOLTAGE] * (130.0f / 30.0f));
    sysmon.battery_voltage = (uint32_t) lroundf(samples[SYSMON_ADC_BATTERY_VOLTAGE] * (340.0f / 240.0f));
    sysmon.motor_current   = (uint32_t) lroundf(samples[SYSMON_ADC_MOTOR_CURRENT]);
    InternalTemperatureSensor_Run_Convert(samples[SYSMON_ADC_TEMPERATURE_P], samples[SYSMON_ADC_TEMPERATURE_C], &sysmon.temperature);

    mainBatteryVoltage = sysmon.battery_voltage;
    motorBatteryVoltage = sysmon.motor_voltage;

    SystemMonitor_Update(&sysmon);
}

static rgb_t statusLeds[4] = { LED_OFF, LED_OFF, LED_OFF, LED_OFF };
static rgb_t ringLeds[RING_LEDS_AMOUNT] = { 0 };

rgb_t LEDController_Read_StatusLED(uint32_t led_idx)
{
    if (led_idx >= ARRAY_SIZE(statusLeds))
    {
        return (rgb_t) LED_OFF;
    }
    else
    {
        return statusLeds[led_idx];
    }
}

rgb_t LEDController_Read_RingLED(uint32_t led_idx)
{
    if (led_idx >= ARRAY_SIZE(ringLeds))
    {
        return (rgb_t) LED_OFF;
    }
    else
    {
        return ringLeds[led_idx];
    }
}

#define MAIN_BATTERY_INDICATOR_LED  0
#define MOTOR_BATTERY_INDICATOR_LED 1
#define BLUETOOTH_INDICATOR_LED     2
#define STATUS_INDICATOR_LED        3

void BluetoothIndicator_Write_LedColor(rgb_t color)
{
    statusLeds[BLUETOOTH_INDICATOR_LED] = color;
}

void BrainStatusIndicator_Write_LedColor(rgb_t color)
{
    statusLeds[STATUS_INDICATOR_LED] = color;
}

float BatteryCalculator_Read_Voltage(BatteryCalculator_Context_t* context)
{
    if (context == &mainBattery)
    {
        return mainBatteryVoltage;
    }
    else if (context == &motorBattery)
    {
        return motorBatteryVoltage;
    }
    else
    {
        ASSERT(0);
    }

    return 0.0f;
}

void BatteryCalculator_Write_Percentage(BatteryCalculator_Context_t* context, uint8_t percent)
{
    if (context == &mainBattery)
    {
        mainBatteryPercentage = percent;
    }
    else if (context == &motorBattery)
    {
        motorBatteryPercentage = percent;
    }
    else
    {
        ASSERT(0);
    }
}

void BatteryCalculator_Write_BatteryPresent(BatteryCalculator_Context_t* context, bool present)
{
    if (context == &mainBattery)
    {
        mainBatteryDetected = present;
    }
    else if (context == &motorBattery)
    {
        motorBatteryDetected = present;
    }
    else
    {
        ASSERT(0);
    }
}

uint8_t BatteryIndicator_Read_Percentage(BatteryIndicator_Context_t* context)
{
    if (context == &mainBatteryIndicator)
    {
        return mainBatteryPercentage;
    }
    else if (context == &motorBatteryIndicator)
    {
        return motorBatteryPercentage;
    }
    else
    {
        ASSERT(0);
    }

    return 0u;
}

BatteryStatus_t BatteryIndicator_Read_Status(BatteryIndicator_Context_t* context)
{
    if (context == &mainBatteryIndicator)
    {
        if (mainBatteryDetected)
        {
            if (BatteryCharger_Run_GetChargerState() == ChargerState_Charging)
            {
                return BatteryStatus_Charging;
            }
            else
            {
                return BatteryStatus_Present;
            }
        }
        else
        {
            return BatteryStatus_NotPresent;
        }
    }
    else if (context == &motorBatteryIndicator)
    {
        return motorBatteryDetected ? BatteryStatus_Present : BatteryStatus_NotPresent;
    }
    else
    {
        ASSERT(0);
    }

    return BatteryStatus_NotPresent;
}

void BatteryIndicator_Write_LedColor(BatteryIndicator_Context_t* context, rgb_t color)
{
    if (context == &mainBatteryIndicator)
    {
        statusLeds[MAIN_BATTERY_INDICATOR_LED] = color;
    }
    else if (context == &motorBatteryIndicator)
    {
        statusLeds[MOTOR_BATTERY_INDICATOR_LED] = color;
    }
    else
    {
        ASSERT(0);
    }
}

void RingLedDisplay_Write_LedColor(uint32_t led_idx, rgb_t color)
{
    ASSERT(led_idx < RING_LEDS_AMOUNT);
    ringLeds[led_idx] = color;
}

void MasterCommunicationInterface_Call_OnMessageReceived(const uint8_t* buffer, size_t bufferSize)
{
    CommunicationObserver_Run_OnMessageReceived();
    MasterCommunication_Run_HandleCommand(buffer, bufferSize);
}

void MasterCommunicationInterface_Call_RxTimeout(void)
{
    CommunicationObserver_Run_OnMessageMissed();
}

void MasterCommunication_Call_SendResponse(const uint8_t* responseBuffer, size_t responseSize)
{
    MasterCommunicationInterface_Run_SetResponse(responseBuffer, responseSize);
}

void CommunicationObserver_Call_ErrorThresholdReached(void)
{
    /* don't try to be clever */
    NVIC_SystemReset();
}

void MasterStatusObserver_Write_MasterStatus(MasterStatus_t status)
{
    masterStatus = status;

    /* TODO this should be moved to a separate component, probably */
    switch (status)
    {
        default:
        case MasterStatus_Unknown:
            portENTER_CRITICAL();
            statusLeds[STATUS_INDICATOR_LED] = (rgb_t) LED_RED;
            portEXIT_CRITICAL();
            break;

        case MasterStatus_Operational:
            portENTER_CRITICAL();
            statusLeds[STATUS_INDICATOR_LED] = (rgb_t) LED_ORANGE;
            portEXIT_CRITICAL();
            break;

        case MasterStatus_Controlled:
            portENTER_CRITICAL();
            statusLeds[STATUS_INDICATOR_LED] = (rgb_t) LED_GREEN;
            portEXIT_CRITICAL();
            break;
    }
}

SystemState_t BrainStatusIndicator_Read_SystemState(void)
{
    SystemState_t systemState = SystemState_Error;
    switch (masterStatus)
    {
        default:
        case MasterStatus_Unknown:
            systemState = SystemState_Startup;
            break;

        case MasterStatus_Operational:
        case MasterStatus_Controlled:
            systemState = SystemState_Operational;
            break;
    }
    return systemState;
}

bool BrainStatusIndicator_Read_BluetoothControllerPresent(void)
{
    return masterStatus == MasterStatus_Controlled;
}

void BluetoothStatusObserver_Write_IsConnected(bool status)
{
    isBleConnected = status;
}

bool BluetoothIndicator_Read_IsConnected(void)
{
    return isBleConnected;
}

BatteryStatusProvider_ChargerStatus_t BatteryStatusProvider_Read_IsMainBatteryCharging(void)
{
    switch (BatteryCharger_Run_GetChargerState())
    {
        case ChargerState_NotPluggedIn:
            return BatteryStatusProvider_ChargerStatus_NotConnected;

        case ChargerState_Charging:
            return BatteryStatusProvider_ChargerStatus_Charging;

        case ChargerState_Charged:
            return BatteryStatusProvider_ChargerStatus_NotCharging;

        default:
            return BatteryStatusProvider_ChargerStatus_Error;
    }
}

uint8_t BatteryStatusProvider_Read_MainBatteryLevel(void)
{
    return mainBatteryPercentage;
}

uint8_t BatteryStatusProvider_Read_MotorBatteryLevel(void)
{
    return motorBatteryDetected ? motorBatteryPercentage : 0u;
}
