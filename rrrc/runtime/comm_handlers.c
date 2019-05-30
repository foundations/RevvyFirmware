/*
 * comm_handlers.c
 *
 * Created: 2019. 05. 30. 14:33:06
 *  Author: Dániel Buga
 */ 

#include "../rrrc_worklogic.h"
 
static Comm_Status_t PingMessageHandler_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

const Comm_CommandHandler_t communicationHandlers[COMM_HANDLER_COUNT] = 
{
    /* other commands */
    [0x00u] = { .Start = &PingMessageHandler_Start, .GetResult = NULL, .Cancel = NULL },
    [0x01u] = { .Start = &VersionProvider_GetHardwareVersion_Start, .GetResult = NULL, .Cancel = NULL },
    [0x02u] = { .Start = &VersionProvider_GetFirmwareVersion_Start, .GetResult = NULL, .Cancel = NULL },
    [0x03u] = { .Start = &BatteryStatusProvider_Start, .GetResult = NULL, .Cancel = NULL },
    [0x04u] = { .Start = &MasterStatusObserver_SetMasterStatus_Start, .GetResult = NULL, .Cancel = NULL },
    [0x05u] = { .Start = &BluetoothStatusObserver_SetBluetoothStatus_Start, .GetResult = NULL, .Cancel = NULL },
    /* [0x06 - 0x0A]: reserved for bootloader */
    
    /* motor commands */
    [0x10u] = { .Start = &MotorPortHandler_GetPortAmount_Start, .GetResult = NULL, .Cancel = NULL },
    [0x11u] = { .Start = &MotorPortHandler_GetPortTypes_Start, .GetResult = NULL, .Cancel = NULL },
    [0x12u] = { .Start = &MotorPortHandler_SetPortType_Start, .GetResult = &MotorPortHandler_SetPortType_GetResult, .Cancel = NULL },
    [0x13u] = { .Start = &MotorPortHandler_SetPortConfig_Start, .GetResult = &MotorPortHandler_SetPortConfig_GetResult, .Cancel = NULL },
    [0x14u] = { .Start = &MotorPortHandler_SetControlValue_Start, .GetResult = NULL, .Cancel = NULL },
    [0x15u] = { .Start = &MotorPortHandler_GetStatus_Start, .GetResult = NULL, .Cancel = NULL },
    
    [0x1Au] = { .Start = &DriveTrain_Set_Start, .GetResult = NULL, .Cancel = NULL },
    [0x1Bu] = { .Start = &DriveTrain_SetControlValue_Start, .GetResult = NULL, .Cancel = NULL },

    /* sensor commands */
    [0x20u] = { .Start = &SensorPortHandler_GetPortAmount_Start, .GetResult = NULL, .Cancel = NULL },
    [0x21u] = { .Start = &SensorPortHandler_GetPortTypes_Start, .GetResult = NULL, .Cancel = NULL },
    [0x22u] = { .Start = &SensorPortHandler_SetPortType_Start, .GetResult = &SensorPortHandler_SetPortType_GetResult, .Cancel = NULL },
    [0x23u] = { .Start = &SensorPortHandler_SetPortConfig_Start, .GetResult = &SensorPortHandler_SetPortConfig_GetResult, .Cancel = NULL },
    [0x24u] = { .Start = &SensorPortHandler_GetSensorData_Start, .GetResult = &SensorPortHandler_GetSensorData_GetResult, .Cancel = NULL },

    /* led ring commands */
    [0x30u] = { .Start = &RingLedDisplay_GetScenarioTypes_Start, .GetResult = NULL, .Cancel = NULL },
    [0x31u] = { .Start = &RingLedDisplay_SetScenarioType_Start, .GetResult = NULL, .Cancel = NULL },
    [0x32u] = { .Start = &RingLedDisplay_GetRingLedAmount_Start, .GetResult = NULL, .Cancel = NULL },
    [0x33u] = { .Start = &RingLedDisplay_SetUserFrame_Start, .GetResult = NULL, .Cancel = NULL },
};

static Comm_Status_t PingMessageHandler_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    return Comm_Status_Ok;
}
