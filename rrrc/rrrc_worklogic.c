/*
 * rrrc_worklogic.c
 *
 * Created: 2/14/2019 11:38:22 AM
 *  Author: User
 */ 
 
#include "rrrc_hal.h"
#include "rrrc_worklogic.h"
#include "rrrc_indication.h"

#include <math.h>
#include <string.h>
#include "utils/functions.h"

BatteryCalculator_Context_t mainBattery;
BatteryCalculator_Context_t motorBattery;

BatteryIndicator_Context_t mainBatteryIndicator;
BatteryIndicator_Context_t motorBatteryIndicator;

static bool mainBatteryDetected;
static bool motorBatteryDetected;
static float mainBatteryVoltage;
static float motorBatteryVoltage;
static uint8_t mainBatteryPercentage;
static uint8_t motorBatteryPercentage;

static MotorThermalModel_t motorThermalModels[6] = {
    { .idx = 0u },
    { .idx = 1u },
    { .idx = 2u },
    { .idx = 3u },
    { .idx = 4u },
    { .idx = 5u }
};

static MotorPort_t motorPorts[] = 
{
    {
        .port_idx = 0u,
        .led = M5_GREEN_LED,
        .enc0 = M5_ENC_A,
        .enc1 = M5_ENC_B,

        .driver_idx = M5_DRIVER_IDX,
        .driver_channel = M5_DRIVER_CHANNEL
    },
    {
        .port_idx = 1u,
        .led = M4_GREEN_LED,
        .enc0 = M4_ENC_A,
        .enc1 = M4_ENC_B,

        .driver_idx = M4_DRIVER_IDX,
        .driver_channel = M4_DRIVER_CHANNEL
    },
    {
        .port_idx = 2u,
        .led = M3_GREEN_LED,
        .enc0 = M3_ENC_A,
        .enc1 = M3_ENC_B,

        .driver_idx = M3_DRIVER_IDX,
        .driver_channel = M3_DRIVER_CHANNEL
    },
    {
        .port_idx = 3u,
        .led = M0_GREEN_LED,
        .enc0 = M0_ENC_A,
        .enc1 = M0_ENC_B,

        .driver_idx = M0_DRIVER_IDX,
        .driver_channel = M0_DRIVER_CHANNEL
    },
    {
        .port_idx = 4u,
        .led = M1_GREEN_LED,
        .enc0 = M1_ENC_A,
        .enc1 = M1_ENC_B,

        .driver_idx = M1_DRIVER_IDX,
        .driver_channel = M1_DRIVER_CHANNEL
    },
    {
        .port_idx = 5u,
        .led = M2_GREEN_LED,
        .enc0 = M2_ENC_A,
        .enc1 = M2_ENC_B,

        .driver_idx = M2_DRIVER_IDX,
        .driver_channel = M2_DRIVER_CHANNEL
    }
};

#define MOTOR_DRIVER_INIT(i)                              \
{                                                         \
    .idx = i,                                             \
                                                          \
    .fault = MOTOR_DRIVER_## i ## _FAULT,                 \
    .n_sleep = MOTOR_DRIVER_## i ## _EN,                  \
                                                          \
    .pwm_a1 = MOTOR_DRIVER_## i ##_CH_A_PWM0_PIN,         \
    .pwm_a2 = MOTOR_DRIVER_## i ##_CH_A_PWM1_PIN,         \
    .pwm_b1 = MOTOR_DRIVER_## i ##_CH_B_PWM0_PIN,         \
    .pwm_b2 = MOTOR_DRIVER_## i ##_CH_B_PWM1_PIN,         \
                                                          \
    .pwm_a1_timer = MOTOR_DRIVER_## i ##_CH_A_PWM0_TIMER, \
    .pwm_a2_timer = MOTOR_DRIVER_## i ##_CH_A_PWM1_TIMER, \
    .pwm_b1_timer = MOTOR_DRIVER_## i ##_CH_B_PWM0_TIMER, \
    .pwm_b2_timer = MOTOR_DRIVER_## i ##_CH_B_PWM1_TIMER, \
                                                          \
    .pwm_a1_ch = MOTOR_DRIVER_## i ##_CH_A_PWM0_CH,       \
    .pwm_a2_ch = MOTOR_DRIVER_## i ##_CH_A_PWM1_CH,       \
    .pwm_b1_ch = MOTOR_DRIVER_## i ##_CH_B_PWM0_CH,       \
    .pwm_b2_ch = MOTOR_DRIVER_## i ##_CH_B_PWM1_CH,       \
}

static MotorDriver_8833_t motorDrivers[] = {
    MOTOR_DRIVER_INIT(0),
    MOTOR_DRIVER_INIT(1),
    MOTOR_DRIVER_INIT(2)
};

static SensorPort_t sensorPorts[] = 
{
    {
        .port_idx = 0u,
        .led0 = S3_LED_GREEN,
        .led1 = S3_LED_YELLOW,
        .gpio0 = S3_GPIO_OUT,
        .gpio1 = S3_GPIO_IN,
        .vccio = S3_IOVCC,
        .i2c_hw = I2C3_SERCOM
    },
    {
        .port_idx = 1u,
        .led0 = S2_LED_GREEN,
        .led1 = S2_LED_YELLOW,
        .gpio0 = S2_GPIO_OUT,
        .gpio1 = S2_GPIO_IN,
        .vccio = S2_IOVCC,
        .i2c_hw = I2C2_SERCOM
    },
    {
        .port_idx = 2u,
        .led0 = S1_LED_GREEN,
        .led1 = S1_LED_YELLOW,
        .gpio0 = S1_GPIO_OUT,
        .gpio1 = S1_GPIO_IN,
        .vccio = S1_IOVCC,
        .i2c_hw = I2C1_SERCOM
    },
    {
        .port_idx = 3u,
        .led0 = S0_LED_GREEN,
        .led1 = S0_LED_YELLOW,
        .gpio0 = S0_GPIO_OUT,
        .gpio1 = S0_GPIO_IN,
        .vccio = S0_IOVCC,
        .i2c_hw = I2C0_SERCOM
    }
};

#define MAX_MOTOR_STATUS_SIZE 10
#define MAX_SENSOR_STATUS_SIZE 4

#define STATUS_SLOT_BATTERY ((uint8_t) 10u)
#define STATUS_SLOT_AXL     ((uint8_t) 11u)
#define STATUS_SLOT_GYRO    ((uint8_t) 12u)
#define STATUS_SLOT_YAW     ((uint8_t) 13u)

static uint8_t motor_status[6][MAX_MOTOR_STATUS_SIZE + 1] = {0};
static uint8_t sensor_status[4][MAX_SENSOR_STATUS_SIZE + 1] = {0};
static uint8_t gyro_status[6];
static uint8_t axl_status[6];
static uint8_t yaw_status[8];
static bool status_changed[32] = {0};

_Static_assert(sizeof(axl_status) == sizeof(IMU_RawSample_t), "Accelerometer slot size does not match data size");
_Static_assert(sizeof(gyro_status) == sizeof(IMU_RawSample_t), "Gyroscope slot size does not match data size");

static void ProcessTasks_1ms(void)
{
    for (size_t i = 0u; i < ARRAY_SIZE(motorThermalModels); i++)
    {
        MotorThermalModel_Run_OnUpdate(&motorThermalModels[i]);
    }
    RunnableGroup_1ms();
}

static void ProcessTasks_10ms(uint8_t offset)
{
    switch (offset)
    {
        case 0u:
            RunnableGroup_10ms_offset0();
            break;

        case 1u:
            RunnableGroup_10ms_offset1();
            break;
            
        case 2u:
            RunnableGroup_10ms_offset2();
            break;
            
        case 3u:
            RunnableGroup_10ms_offset3();
            break;
            
        case 4u:
            RunnableGroup_10ms_offset4();
            break;
            
        case 5u:
            RunnableGroup_10ms_offset5();
            break;
            
        case 6u:
            RunnableGroup_10ms_offset6();
            break;
            
        case 7u:
            RunnableGroup_10ms_offset7();
            break;
            
        case 8u:
            RunnableGroup_10ms_offset8();
            break;
            
        case 9u:
            RunnableGroup_10ms_offset9();
            break;
    }
}

static void ProcessTasks_20ms(uint8_t offset)
{
    switch (offset)
    {
        case 0u:
            RunnableGroup_20ms_offset0();
            break;

        case 1u:
            RunnableGroup_20ms_offset1();
            break;
            
        case 2u:
            MotorPortHandler_Run_PortUpdate(0u);
            MotorDerating_Run_OnUpdate(0u);
            RunnableGroup_20ms_offset2();
            break;
            
        case 3u:
            MotorPortHandler_Run_PortUpdate(1u);
            MotorDerating_Run_OnUpdate(1u);
            RunnableGroup_20ms_offset3();
            break;
            
        case 4u:
            MotorPortHandler_Run_PortUpdate(2u);
            MotorDerating_Run_OnUpdate(2u);
            RunnableGroup_20ms_offset4();
            break;
            
        case 5u:
            MotorPortHandler_Run_PortUpdate(3u);
            MotorDerating_Run_OnUpdate(3u);
            RunnableGroup_20ms_offset5();
            break;
            
        case 6u:
            MotorPortHandler_Run_PortUpdate(4u);
            MotorDerating_Run_OnUpdate(4u);
            RunnableGroup_20ms_offset6();
            break;
            
        case 7u:
            MotorPortHandler_Run_PortUpdate(5u);
            MotorDerating_Run_OnUpdate(5u);
            RunnableGroup_20ms_offset7();
            break;
            
        case 8u:
            MotorDriver_8833_Run_OnUpdate(&motorDrivers[0]);
            MotorDriver_8833_Run_OnUpdate(&motorDrivers[1]);
            MotorDriver_8833_Run_OnUpdate(&motorDrivers[2]);
            RunnableGroup_20ms_offset8();
            break;
            
        case 9u:
            SensorPortHandler_Run_PortUpdate(0u);
            RunnableGroup_20ms_offset9();
            break;

        case 10u:
            SensorPortHandler_Run_PortUpdate(1u);
            RunnableGroup_20ms_offset10();
            break;

        case 11u:
            SensorPortHandler_Run_PortUpdate(2u);
            RunnableGroup_20ms_offset11();
            break;

        case 12u:
            SensorPortHandler_Run_PortUpdate(3u);
            RunnableGroup_20ms_offset12();
            break;

        case 13u:
            RunnableGroup_20ms_offset13();
            break;

        case 14u:
            RunnableGroup_20ms_offset14();
            break;

        case 15u:
            RunnableGroup_20ms_offset15();
            break;

        case 16u:
            RunnableGroup_20ms_offset16();
            break;

        case 17u:
            RunnableGroup_20ms_offset17();
            break;

        case 18u:
            RunnableGroup_20ms_offset18();
            break;

        case 19u:
            RunnableGroup_20ms_offset19();
            break;

        default:
            break;
    }
}

static void ProcessTasks_100ms(void)
{
    BatteryCalculator_Run_Update(&mainBattery);
    BatteryCalculator_Run_Update(&motorBattery);

    BatteryIndicator_Run_Update(&mainBatteryIndicator);
    BatteryIndicator_Run_Update(&motorBatteryIndicator);

    RunnableGroup_100ms();
}

void RRRC_ProcessLogic_Init(void)
{
    system_init();

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

    MasterCommunication_Run_OnInit(&communicationHandlers[0], COMM_HANDLER_COUNT);
    RunnableGroup_OnInit();

    for (size_t i = 0u; i < ARRAY_SIZE(motorThermalModels); i++)
    {
        MotorThermalModel_Run_OnInit(&motorThermalModels[i]);
    }

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

    RingLedDisplay_Run_SelectScenario(RingLedScenario_ColorWheel);

    MotorPortHandler_Run_OnInit(&motorPorts[0], ARRAY_SIZE(motorPorts));
    SensorPortHandler_Run_OnInit(&sensorPorts[0], ARRAY_SIZE(sensorPorts));
    
    MotorDriver_8833_Run_OnGlobalInit();
    MotorDriver_8833_Run_OnInit(&motorDrivers[0]);
    MotorDriver_8833_Run_OnInit(&motorDrivers[1]);
    MotorDriver_8833_Run_OnInit(&motorDrivers[2]);
}

//*********************************************************************************************
void RRRC_ProcessLogic_xTask(void* user)
{
    (void) user;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (uint8_t cycleCounter = 0u;;)
    {
        ProcessTasks_1ms();
    
        ProcessTasks_10ms(cycleCounter % 10);
        ProcessTasks_20ms(cycleCounter % 20);
        if (cycleCounter == 99u)
        {
            ProcessTasks_100ms();
            cycleCounter = 0u;
        }
        else
        {
            cycleCounter++;
        }

        vTaskDelayUntil(&xLastWakeTime, 1u);
    }
}

static uint8_t sensorAdcValues[ARRAY_SIZE(sensorPorts)];
static float motorCurrents[ARRAY_SIZE(motorPorts)];
static float motorPreviousCurrents[ARRAY_SIZE(motorPorts)];
static float motorTemperatures[ARRAY_SIZE(motorPorts)];
static uint16_t motorRawCurrents[ARRAY_SIZE(motorPorts)];

void ADC0_Write_RawChannelData(uint32_t channel_idx, uint16_t adc_data)
{
    switch (channel_idx)
    {
        case M1_ISEN_CH:
            motorRawCurrents[4] = adc_data;
            break;

        case M3_ISEN_CH:
            motorRawCurrents[2] = adc_data;
            break;

        case M4_ISEN_CH:
            motorRawCurrents[1] = adc_data;
            break;

        case S0_ADC_CH:
            sensorAdcValues[3] = adc_data >> 4; /* 12 -> 8 bit */
            break;
    }
}

void ADC1_Write_RawChannelData(uint32_t channel_idx, uint16_t adc_data)
{
    switch (channel_idx)
    {
        case S1_ADC_CH:
            sensorAdcValues[2] = adc_data >> 4; /* 12 -> 8 bit */
            break;

        case S2_ADC_CH:
            sensorAdcValues[1] = adc_data >> 4; /* 12 -> 8 bit */
            break;

        case S3_ADC_CH:
            sensorAdcValues[0] = adc_data >> 4; /* 12 -> 8 bit */
            break;

        case M0_ISEN_CH:
            motorRawCurrents[3] = adc_data;
            break;

        case M2_ISEN_CH:
            motorRawCurrents[5] = adc_data;
            break;

        case M5_ISEN_CH:
            motorRawCurrents[0] = adc_data;
            break;
    }
}

static inline void _update_current(uint8_t idx, float voltage)
{
    motorPreviousCurrents[idx] = motorCurrents[idx];
    motorCurrents[idx] = map_constrained(voltage, 0, 200, 0, 1.66667f) * 0.05f + motorPreviousCurrents[idx] * 0.95f;
}

void ADC0_Write_ChannelVoltage(uint32_t channel_idx, float voltage)
{
    switch (channel_idx)
    {
        case M1_ISEN_CH:
            _update_current(4, voltage);
            break;

        case M3_ISEN_CH:
            _update_current(2, voltage);
            break;

        case M4_ISEN_CH:
            _update_current(1, voltage);
            break;
    }
}

void ADC1_Write_ChannelVoltage(uint32_t channel_idx, float voltage)
{
    switch (channel_idx)
    {
        case ADC_CH_MOT_VOLTAGE:
            motorBatteryVoltage = (uint32_t) lroundf(voltage * (130.0f / 30.0f));
            break;

        case ADC_CH_BAT_VOLTAGE:
            mainBatteryVoltage = (uint32_t) lroundf(voltage * (340.0f / 240.0f));
            break;

        case M0_ISEN_CH:
            _update_current(3, voltage);
            break;

        case M2_ISEN_CH:
            _update_current(5, voltage);
            break;

        case M5_ISEN_CH:
            _update_current(0, voltage);
            break;
    }
}

uint8_t SensorPortHandler_Read_AdcData(uint8_t port_idx)
{
    ASSERT(port_idx < ARRAY_SIZE(sensorPorts));

    return sensorAdcValues[port_idx];
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
            switch (BatteryStatusProvider_Read_IsMainBatteryCharging())
            {
                case ChargerState_Charging: return BatteryStatus_Charging;
                case ChargerState_Fault:    return BatteryStatus_Charging_Fault;
                default:                    return BatteryStatus_Present;
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
                CommunicationObserver_Run_Enable();
                
            default:
                break;
        }
    }

    CommunicationObserver_Run_OnMessageReceived();
    MasterCommunication_Run_HandleCommand(buffer, bufferSize);
}

void MasterCommunication_Call_SendResponse(const uint8_t* responseBuffer, size_t responseSize)
{
    MasterCommunicationInterface_Run_SetResponse(responseBuffer, responseSize);
}

uint8_t BatteryStatusProvider_Read_MainBatteryLevel(void)
{
    return mainBatteryPercentage;
}

uint8_t BatteryStatusProvider_Read_MotorBatteryLevel(void)
{
    return motorBatteryDetected ? motorBatteryPercentage : 0u;
}

static MotorPort_DriveRequest_t motorDriveRequests[ARRAY_SIZE(motorPorts)];
static int8_t driveValues[ARRAY_SIZE(motorPorts)] = {0};
static int8_t deratedDriveValues[ARRAY_SIZE(motorPorts)] = {0};
static bool motorControlledByDriveTrain[ARRAY_SIZE(motorPorts)] = {0};

void DriveTrain_Write_MotorAssigned(uint8_t port_idx, bool isAssigned)
{
    ASSERT(port_idx < ARRAY_SIZE(motorPorts));
    motorControlledByDriveTrain[port_idx] = isAssigned;
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

int8_t MotorDerating_Read_ControlValue(uint8_t motor_idx)
{
    return driveValues[motor_idx];
}

void MotorDerating_Write_ControlValue(uint8_t motor_idx, int8_t control_value)
{
    deratedDriveValues[motor_idx] = control_value;
}

void MotorPortHandler_Write_MotorDriveValue(uint8_t motor_idx, int8_t value)
{
    driveValues[motor_idx] = value;
}

int8_t MotorDriver_8833_Read_DriveRequest_ChannelA(MotorDriver_8833_t* driver)
{
    switch (driver->idx)
    {
        case 0u: return deratedDriveValues[4];
        case 1u: return deratedDriveValues[3];
        case 2u: return deratedDriveValues[2];

        default:
            ASSERT(0);
            return 0;
    }
}

int8_t MotorDriver_8833_Read_DriveRequest_ChannelB(MotorDriver_8833_t* driver)
{
    switch (driver->idx)
    {
        case 0u: return deratedDriveValues[5];
        case 1u: return deratedDriveValues[0];
        case 2u: return deratedDriveValues[1];

        default:
            ASSERT(0);
            return 0;
    }
}

float MotorThermalModel_Read_MotorCurrent(MotorThermalModel_t* model)
{
    return motorCurrents[model->idx];
}

void MotorThermalModel_Write_Temperature(MotorThermalModel_t* model, float temp)
{
    motorTemperatures[model->idx] = temp;
}

float MotorDerating_Read_MotorTemperature(uint8_t motor_idx)
{
    return motorTemperatures[motor_idx];
}

static bool compare_and_copy(uint8_t* pDst, const uint8_t* pSrc, size_t size)
{
    bool equal = true;
    for (uint8_t i = 0u; i < size; i++)
    {
        if (pSrc[i] != pDst[i])
        {
            pDst[i] = pSrc[i];
            equal = false;
        }
    }

    return equal;
}

static bool _update_port(uint8_t* pBuffer, uint8_t* pData, uint8_t dataSize)
{
    bool size_changed = pBuffer[0] != dataSize;
    pBuffer[0] = dataSize;

    bool data_changed = !compare_and_copy(&pBuffer[1u], pData, dataSize);

    return data_changed || size_changed;
}

void MotorPort_Write_PortState(uint8_t port_idx, uint8_t* pData, uint8_t dataSize)
{
    portENTER_CRITICAL();
    ASSERT(dataSize <= MAX_MOTOR_STATUS_SIZE);

    status_changed[port_idx] = _update_port(motor_status[port_idx], pData, dataSize);

    portEXIT_CRITICAL();
}

void SensorPort_Write_PortState(uint8_t port_idx, uint8_t* pData, uint8_t dataSize)
{
    portENTER_CRITICAL();
    ASSERT(dataSize <= MAX_SENSOR_STATUS_SIZE);

    status_changed[port_idx + 6u] = _update_port(sensor_status[port_idx], pData, dataSize);

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
        else if (slot == STATUS_SLOT_BATTERY)
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
        else if (slot == STATUS_SLOT_AXL)
        {
            if (bufferSize >= sizeof(axl_status))
            {
                memcpy(pData, axl_status, sizeof(axl_status));
                *slotDataSize = sizeof(axl_status);
            }
        }
        else if (slot == STATUS_SLOT_GYRO)
        {
            if (bufferSize >= sizeof(gyro_status))
            {
                memcpy(pData, gyro_status, sizeof(gyro_status));
                *slotDataSize = sizeof(gyro_status);
            }
        }
        else if (slot == STATUS_SLOT_YAW)
        {
            if (bufferSize >= sizeof(yaw_status))
            {
                memcpy(pData, yaw_status, sizeof(yaw_status));
                *slotDataSize = sizeof(yaw_status);
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
    status_changed[slot] = false;
    if (slot < 6u)
    {
        uint8_t motor_idx = slot;
        motor_status[motor_idx][0] = 0u;
    }
    else if (slot < 10u)
    {
        uint8_t sensor_idx = slot - 6u;
        sensor_status[sensor_idx][0] = 0u;
    }
    else if (slot == STATUS_SLOT_BATTERY)
    {
        /* nothing to do for battery slot */
    }
    else if (slot == STATUS_SLOT_AXL || slot == STATUS_SLOT_GYRO)
    {
        status_changed[slot] = false;
    }
    else if (slot == STATUS_SLOT_YAW)
    {
        /* yaw may or may not change often, make sure it's value is read */
        status_changed[slot] = true;
    }
    else
    {
        ASSERT(0);
    }
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

void IMU_Write_RawAccelerometerSample(const IMU_RawSample_t* sample)
{
    status_changed[STATUS_SLOT_AXL] = !compare_and_copy(axl_status, (const uint8_t*) sample, sizeof(axl_status));
}

void UpdateMcuStatus_Gyroscope(const IMU_RawSample_t* sample)
{
    status_changed[STATUS_SLOT_GYRO] = !compare_and_copy(gyro_status, (const uint8_t*) sample, sizeof(gyro_status));
}

void UpdateMcuStatus_YawAngle(int32_t angle, int32_t relativeAngle)
{
    uint8_t buffer[8];
    memcpy(buffer, &angle, sizeof(int32_t));
    memcpy(&buffer[4], &relativeAngle, sizeof(int32_t));
    
    status_changed[STATUS_SLOT_YAW] = !compare_and_copy(yaw_status, (const uint8_t*) buffer, sizeof(buffer));
}

void MasterCommunicationInterface_Read_Configuration(MasterCommunicationInterface_Config_t* dst)
{
    dst->rxTimeout = 100u;
    
    MasterCommunication_Run_GetDefaultResponse(&dst->defaultResponseBuffer, &dst->defaultResponseLength);
    MasterCommunication_Run_GetLongRxErrorResponse(&dst->longRxErrorResponseBuffer, &dst->longRxErrorResponseLength);
}
