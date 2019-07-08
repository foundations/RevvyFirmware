/*
 * rrrc_worklogic.c
 *
 * Created: 2/14/2019 11:38:22 AM
 *  Author: User
 */ 
 
#include "rrrc_hal.h"
#include "rrrc_worklogic.h"
#include "rrrc_indication.h"

#include "jscope/jscope.h"

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

static bool masterBooted = false;
static float mcuTemperature;

static MotorPort_t motorPorts[] = 
{
    {
        .port_idx = 0u,
        .led0 = M3LED0,
        .led1 = M3LED1,
        .enc0 = M4ENC0,
        .enc1 = M4ENC1,
        .enc0_timer = &TIMER_TC5,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC5,
        .enc1_timer_event = TIMER_MC1
    },
    {
        .port_idx = 1u,
        .led0 = M4LED0,
        .led1 = M4LED1,
        .enc0 = M5ENC0,
        .enc1 = M5ENC1,
        .enc0_timer = &TIMER_TC1,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC1,
        .enc1_timer_event = TIMER_MC1
    },
    {
        .port_idx = 2u,
        .led0 = M5LED0,
        .led1 = M5LED1,
        .enc0 = M6ENC0,
        .enc1 = M6ENC1,
        .enc0_timer = &TIMER_TC4,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC4,
        .enc1_timer_event = TIMER_MC1
    },
    {
        .port_idx = 3u,
        .led0 = M2LED0,
        .led1 = M2LED1,
        .enc0 = M3ENC0,
        .enc1 = M3ENC1,
        .enc0_timer = &TIMER_TC3,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC3,
        .enc1_timer_event = TIMER_MC1
    },
    {
        .port_idx = 4u,
        .led0 = M1LED0,
        .led1 = M1LED1,
        .enc0 = M2ENC0,
        .enc1 = M2ENC1,
        .enc0_timer = &TIMER_TC6,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC6,
        .enc1_timer_event = TIMER_MC1
    },
    {
        .port_idx = 5u,
        .led0 = M0LED0,
        .led1 = M0LED1,
        .enc0 = M1ENC0,
        .enc1 = M1ENC1,
        .enc0_timer = &TIMER_TC0,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC0,
        .enc1_timer_event = TIMER_MC1
    }
};

static SensorPort_t sensorPorts[] = 
{
    {
        .led0 = S0LED0,
        .led1 = S0LED1,
        .gpio0 = S0GPIO0,
        .gpio1 = S0GPIO1,
        .vccio = S0IOVCC,
        .adc = 3,
        .i2c_hw = I2C4_SERCOM
    },
    {
        .led0 = S1LED0,
        .led1 = S1LED1,
        .gpio0 = S1GPIO0,
        .gpio1 = S1GPIO1,
        .vccio = S1IOVCC,
        .adc = 0,
        .i2c_hw = I2C1_SERCOM
    },
    {
        .led0 = S2LED0,
        .led1 = S2LED1,
        .gpio0 = S2GPIO0,
        .gpio1 = S2GPIO1,
        .vccio = S2IOVCC,
        .adc = 1,
        .i2c_hw = I2C2_SERCOM
    },
    {
        .led0 = S3LED0,
        .led1 = S3LED1,
        .gpio0 = S3GPIO0,
        .gpio1 = S3GPIO1,
        .vccio = S3IOVCC,
        .adc = 2,
        .i2c_hw = I2C3_SERCOM
    }
};

static TB6612FNG_t motorDriver01 = 
{
    .channel_a = 
    {
        .dir0 = M1DIR1,
        .dir1 = M1DIR2,
        .pwm_pin = M0PWM0,
        .pwm_channel = 1
    },
    .channel_b = 
    {
        .dir0 = M2DIR1,
        .dir1 = M2DIR2,
        .pwm_pin = M1PWM0,
        .pwm_channel = 0
    },
    .standby = M12STBY
};
static TB6612FNG_t motorDriver23 = 
{
    .channel_a = 
    {
        .dir0 = M3DIR1,
        .dir1 = M3DIR2,
        .pwm_pin = M2PWM0,
        .pwm_channel = 5
    },
    .channel_b = 
    {
        .dir0 = M4DIR1,
        .dir1 = M4DIR2,
        .pwm_pin = M3PWM0,
        .pwm_channel = 2
    },
    .standby = M34STBY
};
static TB6612FNG_t motorDriver45 = 
{
    .channel_a = 
    {
        .dir0 = M5DIR1,
        .dir1 = M5DIR2,
        .pwm_pin = M4PWM0,
        .pwm_channel = 3
    },
    .channel_b = 
    {
        .dir0 = M6DIR1,
        .dir1 = M6DIR2,
        .pwm_pin = M5PWM0,
        .pwm_channel = 4
    },
    .standby = M56STBY
};

static MasterStatus_t masterStatus;
static BluetoothStatus_t isBleConnected;

static MasterCommunicationInterface_Config_t communicationConfig = 
{
    .rxTimeout = 100u, /* ms */
};

void RRRC_ProcessLogic_xTask(void* user_data);

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

    SystemMonitorPinsInit();

    if (pdPASS != xTaskCreate(RRRC_ProcessLogic_xTask, "RRRC_Main", 1024u, NULL, taskPriority_Main, &xRRRC_Main_xTask))
    {
        result = ERR_FAILURE;
    }

    return result;
}

static void ProcessTasks_10ms(void)
{
    ADC_Run_Update();
    BatteryCharger_Run_Update();
}

static void ProcessTasks_20ms(void)
{
    /* TODO: Different ports may be handled with a time offset from each other to decrease jitter */
    MotorPortHandler_Run_Update();
    for (size_t i = 0u; i < ARRAY_SIZE(motorPorts); i++)
    {
        MotorPortHandler_Run_PortUpdate(i);
    }

    SensorPortHandler_Run_Update();
    for (size_t i = 0u; i < ARRAY_SIZE(sensorPorts); i++)
    {
        SensorPortHandler_Run_PortUpdate(i);
    }
    
    RingLedDisplay_Run_Update();
    LEDController_Run_Update();
    
    MotorDriver_TB661FNG_Run_Update(&motorDriver01);
    MotorDriver_TB661FNG_Run_Update(&motorDriver23);
    MotorDriver_TB661FNG_Run_Update(&motorDriver45);
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
    {
        const uint32_t compatible_hw[] = { COMPATIBLE_HW_VERSIONS };
        const uint32_t hw = FLASH_HEADER->hw_version;
        bool on_compatible_hw = false;
        for (size_t i = 0u; i < ARRAY_SIZE(compatible_hw); i++)
        {
            if (hw == compatible_hw[i])
            {
                on_compatible_hw = true;
                break;
            }
        }

        if (!on_compatible_hw)
        {
            const void * s_rtc_module = (const void *) RTC;
            hri_rtcmode0_set_CTRLB_GP0EN_bit(s_rtc_module);
            hri_rtcmode0_set_CTRLB_GP2EN_bit(s_rtc_module);

            hri_rtc_write_GP_reg(s_rtc_module, 0u, (hri_rtc_gp_reg_t) 0xFFFFFFFFu);
            hri_rtc_write_GP_reg(s_rtc_module, 1u, (hri_rtc_gp_reg_t) 0xFFFFFFFFu);
            hri_rtc_write_GP_reg(s_rtc_module, 2u, (hri_rtc_gp_reg_t) 0xFFFFFFFFu);
            hri_rtc_write_GP_reg(s_rtc_module, 3u, (hri_rtc_gp_reg_t) 0xFFFFFFFFu);
    
            NVIC_SystemReset();
        }
    }

    ADC_Run_OnInit();
    BatteryCharger_Run_OnInit();
    LEDController_Run_OnInit();
    BluetoothIndicator_Run_OnInit();
    BrainStatusIndicator_Run_OnInit();
    
    MasterStatusObserver_Run_OnInit();
    BluetoothStatusObserver_Run_OnInit();

    MasterCommunication_Run_OnInit(&communicationHandlers[0], COMM_HANDLER_COUNT);

    MasterCommunication_Run_GetDefaultResponse(&communicationConfig.defaultResponseBuffer, &communicationConfig.defaultResponseLength);
    MasterCommunication_Run_GetLongRxErrorResponse(&communicationConfig.longRxErrorResponseBuffer, &communicationConfig.longRxErrorResponseLength);

    MasterCommunicationInterface_Run_OnInit(&communicationConfig);
    
    /* 1 cell LiPoly */
    mainBattery.detectionVoltage = 2000.0f;
    mainBattery.minVoltage = 3300.0f;
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
    RingLedDisplay_Run_SelectScenario(RingLedScenario_ColorWheel);
    CommunicationObserver_Run_OnInit();
    
    DriveTrain_Run_OnInit();

    MotorPortHandler_Run_OnInit(&motorPorts[0], ARRAY_SIZE(motorPorts));
    SensorPortHandler_Run_OnInit(&sensorPorts[0], ARRAY_SIZE(sensorPorts));
    
    MotorDriver_TB661FNG_Run_OnInit();
    MotorDriver_TB661FNG_Run_OnDriverInit(&motorDriver01);
    MotorDriver_TB661FNG_Run_OnDriverInit(&motorDriver23);
    MotorDriver_TB661FNG_Run_OnDriverInit(&motorDriver45);

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

uint8_t sensorAdcValues[ARRAY_SIZE(sensorPorts)];

void ADC_Write_RawSamples_ADC0(uint16_t samples[4])
{
    for (uint32_t i = 0u; i < ARRAY_SIZE(sensorPorts); i++)
    {
        sensorAdcValues[i] = samples[i] >> 4; /* 12 -> 8 bit */
    }
}

uint8_t SensorPortHandler_Read_AdcData(uint8_t port_idx)
{
    ASSERT(port_idx < ARRAY_SIZE(sensorPorts));

    return sensorAdcValues[port_idx];
}

void ADC_Write_Samples_ADC1(float samples[5])
{
    uint32_t motor_voltage   = (uint32_t) lroundf(samples[SYSMON_ADC_MOTOR_VOLTAGE] * (130.0f / 30.0f));
    uint32_t battery_voltage = (uint32_t) lroundf(samples[SYSMON_ADC_BATTERY_VOLTAGE] * (340.0f / 240.0f));
//    sysmon.motor_current   = (uint32_t) lroundf(samples[SYSMON_ADC_MOTOR_CURRENT]);

//    float temperature;
    InternalTemperatureSensor_Run_Convert(samples[SYSMON_ADC_TEMPERATURE_P], samples[SYSMON_ADC_TEMPERATURE_C], &mcuTemperature);

    mainBatteryVoltage = battery_voltage;
    motorBatteryVoltage = motor_voltage;
}

static bool statusLedsChanged;
static bool ringLedsChanged;
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
    statusLedsChanged = true;
}

void BrainStatusIndicator_Write_LedColor(rgb_t color)
{
    statusLeds[STATUS_INDICATOR_LED] = color;
    statusLedsChanged = true;
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
        if (!rgb_equals(color, statusLeds[MAIN_BATTERY_INDICATOR_LED]))
        {
            statusLeds[MAIN_BATTERY_INDICATOR_LED] = color;
            statusLedsChanged = true;
        }
    }
    else if (context == &motorBatteryIndicator)
    {
        if (!rgb_equals(color, statusLeds[MOTOR_BATTERY_INDICATOR_LED]))
        {
            statusLeds[MOTOR_BATTERY_INDICATOR_LED] = color;
            statusLedsChanged = true;
        }
    }
    else
    {
        ASSERT(0);
    }
}

bool LEDController_Read_StatusLEDs_Changed(void)
{
    bool changed = statusLedsChanged;
    statusLedsChanged = false;
    return changed;
}

bool LEDController_Read_RingLEDs_Changed(void)
{
    bool changed = ringLedsChanged;
    ringLedsChanged = false;
    return changed;
}

void RingLedDisplay_Write_LedColor(uint32_t led_idx, rgb_t color)
{
    ASSERT(led_idx < RING_LEDS_AMOUNT);
    if (!rgb_equals(ringLeds[led_idx], color))
    {
        ringLeds[led_idx] = color;
        ringLedsChanged = true;
    }
}

void MasterCommunicationInterface_Call_OnMessageReceived(const uint8_t* buffer, size_t bufferSize)
{
    if (bufferSize >= 2u)
    {
        /* TODO: this is not the nicest solution */
        switch (buffer[1])
        {
            case 0x00u: /* ping */
            case 0x04u:
            case 0x05u:
            case 0x12u:
            case 0x13u:
            case 0x14u:
            case 0x1Au:
            case 0x1Bu:
            case 0x22u:
            case 0x23u:
            case 0x31u:
            case 0x33u:
                /* only enable for write commands */
                masterBooted = true;
                CommunicationObserver_Run_Enable();
                
            default:
                break;
        }
    }

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
    if (masterStatus != status)
    {
        masterStatus = status;
        statusLedsChanged = true;

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

void BluetoothStatusObserver_Write_IsConnected(BluetoothStatus_t status)
{
    isBleConnected = status;
}

bool BluetoothIndicator_Read_IsConnected(void)
{
    return isBleConnected == BluetoothStatus_Connected;
}

bool BluetoothIndicator_Read_IsActive(void)
{
    return isBleConnected != BluetoothStatus_Inactive;
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

void* SensorPortHandler_Call_Allocate(size_t size)
{
    return MemoryAllocator_Run_Allocate(size);
}

void SensorPortHandler_Call_Free(void** ptr)
{
    MemoryAllocator_Run_Free(ptr);
}

void* MotorPortHandler_Call_Allocate(size_t size)
{
    return MemoryAllocator_Run_Allocate(size);
}

void MotorPortHandler_Call_Free(void** ptr)
{
    MemoryAllocator_Run_Free(ptr);
}

static MotorPort_DriveRequest_t motorDriveRequests[ARRAY_SIZE(motorPorts)];
static int8_t driveValues[ARRAY_SIZE(motorPorts)] = {0};
static bool motorControlledByDriveTrain[ARRAY_SIZE(motorPorts)] = {0};

void DriveTrain_Write_MotorAssigned(uint8_t port_idx, bool isAssigned)
{
    if (port_idx < ARRAY_SIZE(motorPorts))
    {
        motorControlledByDriveTrain[port_idx] = isAssigned;
    }
}

void MotorPortHandler_Write_DriveRequest(uint8_t port_idx, const MotorPort_DriveRequest_t* command)
{
    if (port_idx < ARRAY_SIZE(motorPorts))
    {
        portENTER_CRITICAL();
        if (!motorControlledByDriveTrain[port_idx])
        {
            motorDriveRequests[port_idx] = *command;
        }
        else
        {
            if (motorDriveRequests[port_idx].type == MotorPort_DriveRequest_Position_Relative)
            {
                if (command->type == MotorPort_DriveRequest_Position)
                {
                    /* allow converting relative request to absolute */
                    /* TODO: motor status should be exposed and this request should be handled by an external component */
                    motorDriveRequests[port_idx] = *command;
                }
            }
        }
        portEXIT_CRITICAL();
    }
}

void DriveTrain_Write_DriveRequest(uint8_t port_idx, const DriveTrain_DriveRequest_t* command)
{
    if (port_idx < ARRAY_SIZE(motorPorts))
    {
        portENTER_CRITICAL();
        switch (command->type)
        {
            case DriveTrain_Request_Power:
                motorDriveRequests[port_idx].type = MotorPort_DriveRequest_Power;
                motorDriveRequests[port_idx].v.pwm = command->v.power;
                break;

            case DriveTrain_Request_Speed:
                motorDriveRequests[port_idx].type = MotorPort_DriveRequest_Speed;
                motorDriveRequests[port_idx].v.speed = command->v.speed;
                break;

            case DriveTrain_Request_Position:
                motorDriveRequests[port_idx].type = MotorPort_DriveRequest_Position_Relative;
                motorDriveRequests[port_idx].v.position = command->v.position;
                break;

            default:
                motorDriveRequests[port_idx].type = MotorPort_DriveRequest_Power;
                motorDriveRequests[port_idx].v.pwm = 0;
                break;
        }
        motorDriveRequests[port_idx].speed_limit = command->speed_limit;
        motorDriveRequests[port_idx].power_limit = command->power_limit;
        portEXIT_CRITICAL();
    }
}

void MotorPortHandler_Read_DriveRequest(uint8_t port_idx, MotorPort_DriveRequest_t* dst)
{
    if (port_idx < ARRAY_SIZE(motorPorts))
    {
        portENTER_CRITICAL();
        *dst = motorDriveRequests[port_idx];
        portEXIT_CRITICAL();
    }
    else
    {
        *dst = (MotorPort_DriveRequest_t) {
            .type = MotorPort_DriveRequest_Power,
            .v.pwm = 0,
            .speed_limit = 0.0f,
            .power_limit = 0.0f
        };
    }
}

void MotorPortHandler_Write_MotorDriveValue(uint8_t port_idx, int8_t value)
{
    driveValues[port_idx] = value;
}

int8_t MotorDriver_TB661FNG_Read_DriveValue_ChannelA(TB6612FNG_t* driver)
{
    if (driver == &motorDriver01)
    {
        return driveValues[5];
    }
    else if (driver == &motorDriver23)
    {
        return driveValues[3];
    }
    else if (driver == &motorDriver45)
    {
        return driveValues[1];
    }
    else
    {
        return 0;
    }
}

int8_t MotorDriver_TB661FNG_Read_DriveValue_ChannelB(TB6612FNG_t* driver)
{
    if (driver == &motorDriver01)
    {
        return driveValues[4];
    }
    else if (driver == &motorDriver23)
    {
        return driveValues[0];
    }
    else if (driver == &motorDriver45)
    {
        return driveValues[2];
    }
    else
    {
        return 0;
    }
}

bool RingLedDisplay_Read_MasterReady(void)
{
    return masterBooted;
}
