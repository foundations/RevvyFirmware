#include "generated_runtime.h"
#include "utils.h"

static ChargerState_t BatteryCharger_ChargerState_BatteryStatusProvider_IsMainBatteryCharging_variable = ChargerState_NotPluggedIn;
static bool CommunicationObserver_Enabled_RingLedDisplay_MasterReady_variable = false;
static BluetoothStatus_t BluetoothStatusObserver_ConnectionStatus_BluetoothIndicator_ConnectionStatus_variable = BluetoothStatus_Inactive;
static Vector3D_t IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue;
static bool IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_overflow = false;
static bool IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_data_valid = false;
static uint16_t ADC0_RawChannelData_ADCDispatcher_ADC0_RawChannelData_array[4] = { 0u, 0u, 0u, 0u };
static uint16_t ADC1_RawChannelData_ADCDispatcher_ADC1_RawChannelData_array[8] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u };
static Voltage_t ADC0_ChannelVoltage_ADCDispatcher_ADC0_ChannelVoltage_array[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
static Voltage_t ADC1_ChannelVoltage_ADCDispatcher_ADC1_ChannelVoltage_array[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static Current_t ADCDispatcher_MotorCurrent_MotorCurrentFilter_RawCurrent_array[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static Voltage_t ADCDispatcher_MainBatteryVoltage_BatteryCalculator_MainBatteryVoltage_variable = 0.0f;
static Voltage_t ADCDispatcher_MotorBatteryVoltage_BatteryCalculator_MotorBatteryVoltage_variable = 0.0f;
static uint8_t ADCDispatcher_Sensor_ADC_SensorPortHandler_AdcData_array[4] = { 0u, 0u, 0u, 0u };

void Runtime_Call_OnInit(void)
{
    ErrorStorage_Run_OnInit();
    ADC0_Run_OnInit();
    ADC1_Run_OnInit();
    ADCDispatcher_Run_OnInit();
    MotorCurrentFilter_Run_OnInit();
    BatteryCharger_Run_OnInit();
    LEDController_Run_OnInit();
    BatteryCalculator_Run_OnInit();
    BluetoothIndicator_Run_OnInit();
    BrainStatusIndicator_Run_OnInit();
    IMU_Run_OnInit();
    HighResolutionTimer_Run_OnInit();
    MasterStatusObserver_Run_OnInit();
    BluetoothStatusObserver_Run_OnInit();
    RingLedDisplay_Run_OnInit();
    CommunicationObserver_Run_OnInit();
    DriveTrain_Run_OnInit();
    GyroscopeOffsetCompensator_Run_OnInit();
    YawAngleTracker_Run_OnInit();
    MasterCommunicationInterface_Run_OnInit();
}

void Runtime_Call_1ms(void)
{
    ADC0_Run_Update();
    ADC1_Run_Update();
    IMU_Run_OnUpdate();
    GyroscopeOffsetCompensator_Run_Update();
    YawAngleTracker_Run_Update();
    ADCDispatcher_Run_Update();
    MotorCurrentFilter_Run_Update();
}

void Runtime_Call_10ms_offset0(void)
{
    BatteryCharger_Run_Update();
    WatchdogFeeder_Run_Feed();
    DriveTrain_Run_Update();
}

void Runtime_Call_10ms_offset1(void)
{
}

void Runtime_Call_10ms_offset2(void)
{
}

void Runtime_Call_10ms_offset3(void)
{
}

void Runtime_Call_10ms_offset4(void)
{
}

void Runtime_Call_10ms_offset5(void)
{
}

void Runtime_Call_10ms_offset6(void)
{
}

void Runtime_Call_10ms_offset7(void)
{
}

void Runtime_Call_10ms_offset8(void)
{
}

void Runtime_Call_10ms_offset9(void)
{
}

void Runtime_Call_20ms_offset0(void)
{
    RingLedDisplay_Run_Update();
}

void Runtime_Call_20ms_offset1(void)
{
    LEDController_Run_Update();
}

void Runtime_Call_20ms_offset2(void)
{
}

void Runtime_Call_20ms_offset3(void)
{
}

void Runtime_Call_20ms_offset4(void)
{
}

void Runtime_Call_20ms_offset5(void)
{
}

void Runtime_Call_20ms_offset6(void)
{
}

void Runtime_Call_20ms_offset7(void)
{
}

void Runtime_Call_20ms_offset8(void)
{
}

void Runtime_Call_20ms_offset9(void)
{
}

void Runtime_Call_20ms_offset10(void)
{
}

void Runtime_Call_20ms_offset11(void)
{
}

void Runtime_Call_20ms_offset12(void)
{
}

void Runtime_Call_20ms_offset13(void)
{
}

void Runtime_Call_20ms_offset14(void)
{
}

void Runtime_Call_20ms_offset15(void)
{
}

void Runtime_Call_20ms_offset16(void)
{
}

void Runtime_Call_20ms_offset17(void)
{
}

void Runtime_Call_20ms_offset18(void)
{
}

void Runtime_Call_20ms_offset19(void)
{
}

void Runtime_Call_100ms(void)
{
    SensorPortHandler_Run_Update();
    MotorPortHandler_Run_Update();
    BatteryCalculator_Run_Update();
    BluetoothIndicator_Run_Update();
    BrainStatusIndicator_Run_Update();
}

void CommunicationObserver_Call_ErrorLimitReached(void)
{
    RestartManager_Run_Reset();
}

void MasterCommunicationInterface_Call_RxTimeout(void)
{
    CommunicationObserver_Run_OnMessageMissed();
}

void* SensorPortHandler_Call_Allocate(size_t size)
{
    void* return_value = MemoryAllocator_Run_Allocate(size);
    return return_value;
}

void SensorPortHandler_Call_Free(void** ptr)
{
    MemoryAllocator_Run_Free(ptr);
}

void* MotorPortHandler_Call_Allocate(size_t size)
{
    void* return_value = MemoryAllocator_Run_Allocate(size);
    return return_value;
}

void MotorPortHandler_Call_Free(void** ptr)
{
    MemoryAllocator_Run_Free(ptr);
}

uint16_t SensorPortHandler_Call_ReadCurrentTicks(void)
{
    uint16_t return_value = HighResolutionTimer_Run_GetTickCount();
    return return_value;
}

void ADC0_Write_ChannelVoltage(uint32_t index, const Voltage_t value)
{
    ASSERT(index < 4);
    ADC0_ChannelVoltage_ADCDispatcher_ADC0_ChannelVoltage_array[index] = value;
}

void ADC0_Write_RawChannelData(uint32_t index, const uint16_t value)
{
    ASSERT(index < 4);
    ADC0_RawChannelData_ADCDispatcher_ADC0_RawChannelData_array[index] = value;
}

void ADC1_Write_ChannelVoltage(uint32_t index, const Voltage_t value)
{
    ASSERT(index < 8);
    ADC1_ChannelVoltage_ADCDispatcher_ADC1_ChannelVoltage_array[index] = value;
}

void ADC1_Write_RawChannelData(uint32_t index, const uint16_t value)
{
    ASSERT(index < 8);
    ADC1_RawChannelData_ADCDispatcher_ADC1_RawChannelData_array[index] = value;
}

void ADCDispatcher_Write_MainBatteryVoltage(const Voltage_t value)
{
    ADCDispatcher_MainBatteryVoltage_BatteryCalculator_MainBatteryVoltage_variable = value;
}

void ADCDispatcher_Write_MotorBatteryVoltage(const Voltage_t value)
{
    ADCDispatcher_MotorBatteryVoltage_BatteryCalculator_MotorBatteryVoltage_variable = value;
}

void ADCDispatcher_Write_MotorCurrent(uint32_t index, const Current_t value)
{
    ASSERT(index < 6);
    ADCDispatcher_MotorCurrent_MotorCurrentFilter_RawCurrent_array[index] = value;
}

void ADCDispatcher_Write_Sensor_ADC(uint32_t index, const uint8_t value)
{
    ASSERT(index < 4);
    ADCDispatcher_Sensor_ADC_SensorPortHandler_AdcData_array[index] = value;
}

void BatteryCharger_Write_ChargerState(const ChargerState_t value)
{
    BatteryCharger_ChargerState_BatteryStatusProvider_IsMainBatteryCharging_variable = value;
}

void BluetoothStatusObserver_Write_ConnectionStatus(const BluetoothStatus_t value)
{
    BluetoothStatusObserver_ConnectionStatus_BluetoothIndicator_ConnectionStatus_variable = value;
}

void CommunicationObserver_Write_Enabled(const bool value)
{
    CommunicationObserver_Enabled_RingLedDisplay_MasterReady_variable = value;
}

void IMU_Write_GyroscopeSample(const Vector3D_t* value)
{
    ASSERT(value != NULL);
    IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_overflow = IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_data_valid;
    IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue = *value;
    IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_data_valid = true;
}

void MotorCurrentFilter_Write_FilteredCurrent(uint32_t index, const Current_t value)
{
    ASSERT(index < 6);
}

Voltage_t ADCDispatcher_Read_ADC0_ChannelVoltage(uint32_t index)
{
    ASSERT(index < 4);
    return ADC0_ChannelVoltage_ADCDispatcher_ADC0_ChannelVoltage_array[index];
}

uint16_t ADCDispatcher_Read_ADC0_RawChannelData(uint32_t index)
{
    ASSERT(index < 4);
    return ADC0_RawChannelData_ADCDispatcher_ADC0_RawChannelData_array[index];
}

Voltage_t ADCDispatcher_Read_ADC1_ChannelVoltage(uint32_t index)
{
    ASSERT(index < 8);
    return ADC1_ChannelVoltage_ADCDispatcher_ADC1_ChannelVoltage_array[index];
}

uint16_t ADCDispatcher_Read_ADC1_RawChannelData(uint32_t index)
{
    ASSERT(index < 8);
    return ADC1_RawChannelData_ADCDispatcher_ADC1_RawChannelData_array[index];
}

void BatteryCalculator_Read_MainBatteryParameters(BatteryConfiguration_t* value)
{
    ASSERT(value != NULL);
    ProjectConfiguration_Constant_MainBatteryParameters(value);
}

Voltage_t BatteryCalculator_Read_MainBatteryVoltage(void)
{
    return ADCDispatcher_MainBatteryVoltage_BatteryCalculator_MainBatteryVoltage_variable;
}

void BatteryCalculator_Read_MotorBatteryParameters(BatteryConfiguration_t* value)
{
    ASSERT(value != NULL);
    ProjectConfiguration_Constant_MotorBatteryParameters(value);
}

Voltage_t BatteryCalculator_Read_MotorBatteryVoltage(void)
{
    return ADCDispatcher_MotorBatteryVoltage_BatteryCalculator_MotorBatteryVoltage_variable;
}

ChargerState_t BatteryStatusProvider_Read_IsMainBatteryCharging(void)
{
    return BatteryCharger_ChargerState_BatteryStatusProvider_IsMainBatteryCharging_variable;
}

BluetoothStatus_t BluetoothIndicator_Read_ConnectionStatus(void)
{
    return BluetoothStatusObserver_ConnectionStatus_BluetoothIndicator_ConnectionStatus_variable;
}

QueueStatus_t GyroscopeOffsetCompensator_Read_AngularSpeeds(Vector3D_t* value)
{
    ASSERT(value != NULL);
    QueueStatus_t return_value = QueueStatus_Empty;
    bool was_overflow = IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_overflow;
    if (IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_data_valid)
    {
        IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_overflow = false;
        *value = IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue;
        IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_data_valid = false;
        if (was_overflow)
        {
            return_value = QueueStatus_Overflow;
        }
        else
        {
            return_value = QueueStatus_Ok;
        }
    }
    return return_value;
}

Current_t MotorCurrentFilter_Read_RawCurrent(uint32_t index)
{
    ASSERT(index < 6);
    return ADCDispatcher_MotorCurrent_MotorCurrentFilter_RawCurrent_array[index];
}

bool RingLedDisplay_Read_MasterReady(void)
{
    return CommunicationObserver_Enabled_RingLedDisplay_MasterReady_variable;
}

uint8_t SensorPortHandler_Read_AdcData(uint32_t index)
{
    ASSERT(index < 4);
    return ADCDispatcher_Sensor_ADC_SensorPortHandler_AdcData_array[index];
}

float YawAngleTracker_Read_SampleTime(void)
{
    return IMU_Constant_SampleTime();
}
