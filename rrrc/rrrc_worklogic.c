/*
 * rrrc_worklogic.c
 *
 * Created: 2/14/2019 11:38:22 AM
 *  Author: User
 */ 
 
#include "rrrc_hal.h"
#include "rrrc_worklogic.h"

#include <math.h>
#include <string.h>

static BlockInfo_t errorStorageBlocks[] = {
    { .base_address = 0x3C000u },
    { .base_address = 0x3E000u },
};

static TaskHandle_t xRRRC_Main_xTask;

static BatteryCalculator_Context_t mainBattery;
static BatteryCalculator_Context_t motorBattery;

BatteryIndicator_Context_t mainBatteryIndicator;
BatteryIndicator_Context_t motorBatteryIndicator;

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
        .led = M3LED0,
        .enc0 = M4ENC0,
        .enc1 = M4ENC1,
        .enc0_timer = &TIMER_TC5,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC5,
        .enc1_timer_event = TIMER_MC1
    },
    {
        .port_idx = 1u,
        .led = M4LED0,
        .enc0 = M5ENC0,
        .enc1 = M5ENC1,
        .enc0_timer = &TIMER_TC1,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC1,
        .enc1_timer_event = TIMER_MC1
    },
    {
        .port_idx = 2u,
        .led = M5LED0,
        .enc0 = M6ENC0,
        .enc1 = M6ENC1,
        .enc0_timer = &TIMER_TC4,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC4,
        .enc1_timer_event = TIMER_MC1
    },
    {
        .port_idx = 3u,
        .led = M2LED0,
        .enc0 = M3ENC0,
        .enc1 = M3ENC1,
        .enc0_timer = &TIMER_TC3,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC3,
        .enc1_timer_event = TIMER_MC1
    },
    {
        .port_idx = 4u,
        .led = M1LED0,
        .enc0 = M2ENC0,
        .enc1 = M2ENC1,
        .enc0_timer = &TIMER_TC6,
        .enc0_timer_event = TIMER_MC0,
        .enc1_timer = &TIMER_TC6,
        .enc1_timer_event = TIMER_MC1
    },
    {
        .port_idx = 5u,
        .led = M0LED0,
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
        .port_idx = 0u,
        .led0 = S0LED0,
        .led1 = S0LED1,
        .gpio0 = S0GPIO0,
        .gpio1 = S0GPIO1,
        .vccio = S0IOVCC,
        .i2c_hw = I2C4_SERCOM
    },
    {
        .port_idx = 1u,
        .led0 = S1LED0,
        .led1 = S1LED1,
        .gpio0 = S1GPIO0,
        .gpio1 = S1GPIO1,
        .vccio = S1IOVCC,
        .i2c_hw = I2C1_SERCOM
    },
    {
        .port_idx = 2u,
        .led0 = S2LED0,
        .led1 = S2LED1,
        .gpio0 = S2GPIO0,
        .gpio1 = S2GPIO1,
        .vccio = S2IOVCC,
        .i2c_hw = I2C2_SERCOM
    },
    {
        .port_idx = 3u,
        .led0 = S3LED0,
        .led1 = S3LED1,
        .gpio0 = S3GPIO0,
        .gpio1 = S3GPIO1,
        .vccio = S3IOVCC,
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

static BluetoothStatus_t isBleConnected;

static MasterCommunicationInterface_Config_t communicationConfig = 
{
    .rxTimeout = 100u, /* ms */
};

#define MAX_MOTOR_STATUS_SIZE 10
#define MAX_SENSOR_STATUS_SIZE 4

static uint8_t motor_status[6][MAX_MOTOR_STATUS_SIZE + 1] = {0};
static uint8_t sensor_status[4][MAX_SENSOR_STATUS_SIZE + 1] = {0};
static bool status_changed[32] = {0};

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
    (void) user;

    ErrorStorage_Run_OnInit(&errorStorageBlocks[0], ARRAY_SIZE(errorStorageBlocks));

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
            RestartManager_Run_RebootToBootloader();
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
    mainBattery.minVoltage = 3400.0f;
    mainBattery.maxVoltage = 4100.0f;

    BatteryCalculator_Run_OnInit(&mainBattery);
    BatteryIndicator_Run_OnInit(&mainBatteryIndicator);
    
    /* 6xAA rechargeable */
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
        if (mainBatteryPercentage != percent)
        {
            mainBatteryPercentage = percent;
        }
    }
    else if (context == &motorBattery)
    {
        if (motorBatteryPercentage != percent)
        {
            motorBatteryPercentage = percent;
        }
    }
    else
    {
        ASSERT(0);
    }
    status_changed[10u] = true;
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

void MasterCommunicationInterface_Call_OnMessageReceived(const uint8_t* buffer, size_t bufferSize)
{
    if (bufferSize >= 2u)
    {
        /* TODO: this is not the nicest solution */
        switch (buffer[1])
        {
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
            case 0x3Au:
            case 0x3Bu:
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
    RestartManager_Run_Reset();
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
    ASSERT(port_idx < ARRAY_SIZE(motorPorts));
    motorControlledByDriveTrain[port_idx] = isAssigned;
}

void MotorPortHandler_Write_DriveRequest(uint8_t port_idx, const MotorPort_DriveRequest_t* command)
{
    ASSERT(port_idx < ARRAY_SIZE(motorPorts));

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

void DriveTrain_Write_DriveRequest(uint8_t port_idx, const DriveTrain_DriveRequest_t* command)
{
    ASSERT(port_idx < ARRAY_SIZE(motorPorts));

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
            ASSERT(0);
            motorDriveRequests[port_idx].type = MotorPort_DriveRequest_Power;
            motorDriveRequests[port_idx].v.pwm = 0;
            break;
    }
    motorDriveRequests[port_idx].speed_limit = command->speed_limit;
    motorDriveRequests[port_idx].power_limit = command->power_limit;
    portEXIT_CRITICAL();
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
        ASSERT(0);
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

void MotorPort_Write_PortState(uint8_t port_idx, uint8_t* pData, uint8_t dataSize)
{
    portENTER_CRITICAL();
    ASSERT(dataSize <= MAX_MOTOR_STATUS_SIZE);

    bool changed = motor_status[port_idx][0] != dataSize;
    motor_status[port_idx][0] = dataSize;
    for (uint8_t i = 0u; i < dataSize; i++)
    {
        if (motor_status[port_idx][i + 1u] != pData[i])
        {
            motor_status[port_idx][i + 1u] = pData[i];
            changed = true;
        }
    }

    uint8_t slot = port_idx;
    status_changed[slot] = changed;

    portEXIT_CRITICAL();
}

void SensorPort_Write_PortState(uint8_t port_idx, uint8_t* pData, uint8_t dataSize)
{
    portENTER_CRITICAL();
    ASSERT(dataSize <= MAX_SENSOR_STATUS_SIZE);

    bool changed = dataSize != sensor_status[port_idx][0];
    sensor_status[port_idx][0] = dataSize;
    for (uint8_t i = 0u; i < dataSize; i++)
    {
        if (sensor_status[port_idx][i + 1u] != pData[i])
        {
            sensor_status[port_idx][i + 1u] = pData[i];
            changed = true;
        }
    }
    
    uint8_t slot = port_idx + 6u;
    status_changed[slot] = changed;

    portEXIT_CRITICAL();
}

void McuStatusCollector_Read_SlotData(uint8_t slot, uint8_t* pData, uint8_t bufferSize, uint8_t* slotDataSize)
{
    portENTER_CRITICAL();
    ASSERT(slot < ARRAY_SIZE(status_changed));

    *slotDataSize = 0u;

    if (status_changed[slot])
    {
        if (slot < 6u)
        {
            uint8_t motor_idx = slot;
            if (bufferSize >= motor_status[motor_idx][0])
            {
                *slotDataSize = motor_status[motor_idx][0];
                memcpy(pData, &motor_status[motor_idx][1], motor_status[motor_idx][0]);
            }
        }
        else if (slot < 10u)
        {
            uint8_t sensor_idx = slot - 6u;
            if (bufferSize >= sensor_status[sensor_idx][0])
            {
                *slotDataSize = sensor_status[sensor_idx][0];
                memcpy(pData, &sensor_status[sensor_idx][1], sensor_status[sensor_idx][0]);
            }
        }
        else if (slot == 10u)
        {
            /* battery */
            if (bufferSize >= 4u)
            {
                pData[0] = BatteryIndicator_Read_Status(&mainBatteryIndicator);
                pData[1] = mainBatteryPercentage;
                pData[2] = BatteryIndicator_Read_Status(&motorBatteryIndicator);
                pData[3] = motorBatteryPercentage;
                *slotDataSize = 4u;
            }
        }
        else
        {
            ASSERT(0);
        }
    }
    portEXIT_CRITICAL();
}

void McuStatusCollector_Call_ClearSlotData(uint8_t slot)
{
    portENTER_CRITICAL();
    ASSERT(slot < ARRAY_SIZE(status_changed));

    status_changed[slot] = false;
    portEXIT_CRITICAL();
}

void McuStatusCollectorWrapper_Run_ResetSlots(void)
{
    McuStatusCollector_Run_ResetSlots();
    
    memset(motor_status, 0u, sizeof(motor_status));
    memset(sensor_status, 0u, sizeof(sensor_status));
}

void McuStatusCollectorWrapper_Run_EnableSlot(uint8_t slot)
{
    McuStatusCollector_Run_EnableSlot(slot);
}

void McuStatusCollectorWrapper_Run_DisableSlot(uint8_t slot)
{
    McuStatusCollector_Run_DisableSlot(slot);
}

void McuStatusCollectorWrapper_Run_ReadData(uint8_t* pData, uint8_t bufferSize, uint8_t* dataSize)
{
    McuStatusCollector_Run_ReadData(pData, bufferSize, dataSize);
}
