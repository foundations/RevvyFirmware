#include "rrrc_hal.h"
#include "rrrc_worklogic.h"
#include "rrrc_indication.h"

#include <math.h>
#include <string.h>
#include "utils/functions.h"

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
    Runtime_Call_1ms();
}

static void ProcessTasks_10ms(uint8_t offset)
{
    switch (offset)
    {
        case 0u:
            Runtime_Call_10ms_offset0();
            break;

        case 1u:
            Runtime_Call_10ms_offset1();
            break;
            
        case 2u:
            MotorPortHandler_Run_PortUpdate(4u);
            MotorPortHandler_Run_PortUpdate(5u);
            MotorDriver_8833_Run_OnUpdate(&motorDrivers[0]);
            Runtime_Call_10ms_offset2();
            break;

        case 3u:
            MotorPortHandler_Run_PortUpdate(1u);
            MotorPortHandler_Run_PortUpdate(2u);
            MotorDriver_8833_Run_OnUpdate(&motorDrivers[2]);
            Runtime_Call_10ms_offset3();
            break;

        case 4u:
            MotorPortHandler_Run_PortUpdate(0u);
            MotorPortHandler_Run_PortUpdate(3u);
            MotorDriver_8833_Run_OnUpdate(&motorDrivers[1]);
            Runtime_Call_10ms_offset4();
            break;
            
        case 5u:
            SensorPortHandler_Run_PortUpdate(0u);
            Runtime_Call_10ms_offset5();
            break;
            
        case 6u:
            SensorPortHandler_Run_PortUpdate(1u);
            Runtime_Call_10ms_offset6();
            break;
            
        case 7u:
            SensorPortHandler_Run_PortUpdate(2u);
            Runtime_Call_10ms_offset7();
            break;
            
        case 8u:
            SensorPortHandler_Run_PortUpdate(3u);
            Runtime_Call_10ms_offset8();
            break;
            
        case 9u:
            Runtime_Call_10ms_offset9();
            break;
    }
}

static void ProcessTasks_20ms(uint8_t offset)
{
    switch (offset)
    {
        case 0u:
            Runtime_Call_20ms_offset0();
            break;

        case 1u:
            Runtime_Call_20ms_offset1();
            break;
            
        case 2u:
            Runtime_Call_20ms_offset2();
            break;

        case 3u:
            Runtime_Call_20ms_offset3();
            break;

        case 4u:
            Runtime_Call_20ms_offset4();
            break;

        case 5u:
            Runtime_Call_20ms_offset5();
            break;

        case 6u:
            Runtime_Call_20ms_offset6();
            break;

        case 7u:
            Runtime_Call_20ms_offset7();
            break;

        case 8u:
            Runtime_Call_20ms_offset8();
            break;

        case 9u:
            Runtime_Call_20ms_offset9();
            break;

        case 10u:
            Runtime_Call_20ms_offset10();
            break;

        case 11u:
            Runtime_Call_20ms_offset11();
            break;

        case 12u:
            Runtime_Call_20ms_offset12();
            break;

        case 13u:
            Runtime_Call_20ms_offset13();
            break;

        case 14u:
            Runtime_Call_20ms_offset14();
            break;

        case 15u:
            Runtime_Call_20ms_offset15();
            break;

        case 16u:
            Runtime_Call_20ms_offset16();
            break;

        case 17u:
            Runtime_Call_20ms_offset17();
            break;

        case 18u:
            Runtime_Call_20ms_offset18();
            break;

        case 19u:
            Runtime_Call_20ms_offset19();
            break;

        default:
            break;
    }
}

static void ProcessTasks_100ms(void)
{
    MotorPortHandler_Run_Update();
    SensorPortHandler_Run_Update();

    Runtime_Call_100ms();
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
    Runtime_Call_OnInit();

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

void MasterCommunicationInterface_Call_OnMessageReceived(MasterMessage_t message)
{
    if (message.size >= 2u)
    {
        /* TODO: this is not the nicest solution */
        switch (message.payload[1])
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
    MasterCommunication_Run_HandleCommand(message.payload, message.size);
}

void MasterCommunication_Call_SendResponse(const uint8_t* responseBuffer, size_t responseSize)
{
    MasterCommunicationInterface_Run_SetResponse((MasterMessage_t) {responseBuffer, responseSize} );
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
    ASSERT(port_idx < ARRAY_SIZE(motorPorts));
     
    portENTER_CRITICAL();
    *dst = motorDriveRequests[port_idx];
    portEXIT_CRITICAL();
}

void MotorDerating_Write_DeratedControlValue(uint32_t index, const int8_t value)
{
    driveValues[index] = value;
}

int8_t MotorDriver_8833_Read_DriveRequest_ChannelA(MotorDriver_8833_t* driver)
{
    switch (driver->idx)
    {
        case 0u: return driveValues[4];
        case 1u: return driveValues[3];
        case 2u: return driveValues[2];

        default:
            ASSERT(0);
            return 0;
    }
}

int8_t MotorDriver_8833_Read_DriveRequest_ChannelB(MotorDriver_8833_t* driver)
{
    switch (driver->idx)
    {
        case 0u: return driveValues[5];
        case 1u: return driveValues[0];
        case 2u: return driveValues[1];

        default:
            ASSERT(0);
            return 0;
    }
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
    ASSERT(dataSize <= MAX_MOTOR_STATUS_SIZE);

    portENTER_CRITICAL();
    status_changed[port_idx] = _update_port(motor_status[port_idx], pData, dataSize);
    portEXIT_CRITICAL();
}

void SensorPort_Write_PortState(uint8_t port_idx, uint8_t* pData, uint8_t dataSize)
{
    ASSERT(dataSize <= MAX_SENSOR_STATUS_SIZE);

    portENTER_CRITICAL();
    status_changed[port_idx + 6u] = _update_port(sensor_status[port_idx], pData, dataSize);
    portEXIT_CRITICAL();
}

void McuStatusCollector_Read_SlotData(uint8_t slot, uint8_t* pData, uint8_t bufferSize, uint8_t* slotDataSize)
{
    ASSERT(slot < ARRAY_SIZE(status_changed));
 
    portENTER_CRITICAL();
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
                pData[0] = 0;
                pData[1] = 0;
                pData[2] = 0;
                pData[3] = 0;
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
    dst->rx_timeout = 100u;
    
    MasterCommunication_Run_GetDefaultResponse(&dst->default_response.payload, &dst->default_response.size);
    MasterCommunication_Run_GetLongRxErrorResponse(&dst->rx_overflow_response.payload, &dst->rx_overflow_response.size);
}
