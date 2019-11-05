/*
 * comm_handlers.c
 *
 * Created: 2019. 05. 30. 14:33:06
 *  Author: D�niel Buga
 */ 

#include "../rrrc_worklogic.h"
 
 #include <utils_assert.h>

/* These constants are common between bootloader and application */
#define OPERATION_MODE_BOOTLOADER   ((uint8_t) 0xBBu)
#define OPERATION_MODE_APPLICATION  ((uint8_t) 0xAAu)
 
static Comm_Status_t PingMessageHandler_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
static Comm_Status_t GetOperationMode_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
static Comm_Status_t RebootToBootloader_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

const Comm_CommandHandler_t communicationHandlers[COMM_HANDLER_COUNT] = 
{
    /* other commands */
    [0x00u] = { .Start = &PingMessageHandler_Start, .GetResult = NULL, .Cancel = NULL },
    [0x01u] = { .Start = &VersionProvider_GetHardwareVersion_Start, .GetResult = NULL, .Cancel = NULL },
    [0x02u] = { .Start = &VersionProvider_GetFirmwareVersion_Start, .GetResult = NULL, .Cancel = NULL },
    [0x04u] = { .Start = &MasterStatusObserver_SetMasterStatus_Start, .GetResult = NULL, .Cancel = NULL },
    [0x05u] = { .Start = &BluetoothStatusObserver_SetBluetoothStatus_Start, .GetResult = NULL, .Cancel = NULL },

    /* [0x06 - 0x0A]: reserved for bootloader */
    [0x06u] = { .Start = &GetOperationMode_Start, .GetResult = NULL, .Cancel = NULL },
    [0x0Bu] = { .Start = &RebootToBootloader_Start, .GetResult = NULL, .Cancel = NULL },
    
    /* motor commands */
    [0x10u] = { .Start = &MotorPortHandler_GetPortAmount_Start, .GetResult = NULL, .Cancel = NULL },
    [0x11u] = { .Start = &MotorPortHandler_GetPortTypes_Start, .GetResult = NULL, .Cancel = NULL },
    [0x12u] = { .Start = &MotorPortHandler_SetPortType_Start, .GetResult = &MotorPortHandler_SetPortType_GetResult, .Cancel = NULL },
    [0x13u] = { .Start = &MotorPortHandler_SetPortConfig_Start, .GetResult = &MotorPortHandler_SetPortConfig_GetResult, .Cancel = NULL },
    [0x14u] = { .Start = &MotorPortHandler_SetControlValue_Start, .GetResult = NULL, .Cancel = NULL },
    [0x15u] = { .Start = &MotorPortHandler_GetStatus_Start, .GetResult = NULL, .Cancel = NULL },
    
    [0x1Au] = { .Start = &DriveTrain_Configure_Start, .GetResult = NULL, .Cancel = NULL },
    [0x1Bu] = { .Start = &DriveTrain_Control_Start, .GetResult = NULL, .Cancel = NULL },

    /* sensor commands */
    [0x20u] = { .Start = &SensorPortHandler_GetPortAmount_Start, .GetResult = NULL, .Cancel = NULL },
    [0x21u] = { .Start = &SensorPortHandler_GetPortTypes_Start, .GetResult = NULL, .Cancel = NULL },
    [0x22u] = { .Start = &SensorPortHandler_SetPortType_Start, .GetResult = &SensorPortHandler_SetPortType_GetResult, .Cancel = NULL },
    [0x23u] = { .Start = &SensorPortHandler_SetPortConfig_Start, .GetResult = &SensorPortHandler_SetPortConfig_GetResult, .Cancel = NULL },
    [0x24u] = { .Start = &SensorPortHandler_GetSensorData_Start, .GetResult = &SensorPortHandler_GetSensorData_GetResult, .Cancel = NULL },

    /* led ring commands */
    [0x30u] = { .Start = &CommWrapper_LedDisplay_GetScenarioTypes_Start, .GetResult = NULL, .Cancel = NULL },
    [0x31u] = { .Start = &CommWrapper_LedDisplay_SetScenarioType_Start, .GetResult = NULL, .Cancel = NULL },
    [0x32u] = { .Start = &CommWrapper_LedDisplay_GetRingLedAmount_Start, .GetResult = NULL, .Cancel = NULL },
    [0x33u] = { .Start = &CommWrapper_LedDisplay_SetUserFrame_Start, .GetResult = NULL, .Cancel = NULL },

    /* MCU status updater commands */
    [0x3Au] = { .Start = &McuStatusCollectorWrapper_Reset_Start, .GetResult = NULL, .Cancel = NULL },
    [0x3Bu] = { .Start = &McuStatusCollectorWrapper_ControlSlot_Start, .GetResult = NULL, .Cancel = NULL },
    [0x3Cu] = { .Start = &McuStatusCollectorWrapper_ReadStatus_Start, .GetResult = NULL, .Cancel = NULL },

    /* Error storage commands */
    [0x3Du] = { .Start = &ErrorStorageWrapper_ReadCount_Start, .GetResult = NULL, .Cancel = NULL }, /* Read stored error count */
    [0x3Eu] = { .Start = &ErrorStorageWrapper_ReadErrors_Start, .GetResult = NULL, .Cancel = NULL }, /* Read errors starting with the given index */
    [0x3Fu] = { .Start = &ErrorStorageWrapper_ClearMemory_Start, .GetResult = NULL, .Cancel = NULL }, /* Clear error memory */
    [0x40u] = { .Start = &ErrorStorageWrapper_StoreTestError_Start, .GetResult = NULL, .Cancel = NULL}, /* Record a test error */
};

static Comm_Status_t PingMessageHandler_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    return Comm_Status_Ok;
}

static Comm_Status_t GetOperationMode_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;

    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    ASSERT(responseBufferSize >= 1u);
    *response = OPERATION_MODE_APPLICATION;
    *responseCount = 1u;
    return Comm_Status_Ok;
}

static Comm_Status_t RebootToBootloader_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    RestartManager_Run_RebootToBootloader();

    /* will not be reached, device will NACK after this point */
    return Comm_Status_Ok;
}
