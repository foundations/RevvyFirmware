#include "generated_runtime.h"
#include "utils.h"

static ChargerState_t BatteryCharger_ChargerState_databuffer = ChargerState_NotPluggedIn;
static bool CommunicationObserver_Enabled_databuffer = false;
static BluetoothStatus_t BluetoothStatusObserver_ConnectionStatus_databuffer = BluetoothStatus_Inactive;
static Vector3D_t IMU_GyroscopeSample_databuffer = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
static bool IMU_GyroscopeDataReady_databuffer = false;
static uint16_t ADC0_RawChannelData_databuffer[4] = { 0u, 0u, 0u, 0u };
static uint16_t ADC1_RawChannelData_databuffer[8] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u };
static Voltage_t ADC0_ChannelVoltage_databuffer[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
static Voltage_t ADC1_ChannelVoltage_databuffer[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static Current_t ADCDispatcher_MotorCurrent_databuffer[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static Voltage_t ADCDispatcher_MainBatteryVoltage_databuffer = 0.0f;
static Voltage_t ADCDispatcher_MotorBatteryVoltage_databuffer = 0.0f;
static uint8_t ADCDispatcher_Sensor_ADC_databuffer[4] = { 0u, 0u, 0u, 0u };
static Current_t MotorCurrentFilter_FilteredCurrent_databuffer[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

void RunnableGroup_OnInit(void)
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

void RunnableGroup_1ms(void)
{
    ADC0_Run_Update();
    ADC1_Run_Update();
    IMU_Run_OnUpdate();
    GyroscopeOffsetCompensator_Run_Update();
    YawAngleTracker_Run_Update();
    ADCDispatcher_Run_Update();
    MotorCurrentFilter_Run_Update();
}

void RunnableGroup_10ms_offset0(void)
{
    BatteryCharger_Run_Update();
}

void RunnableGroup_10ms_offset1(void)
{
    WatchdogFeeder_Run_Feed();
}

void RunnableGroup_10ms_offset2(void)
{
}

void RunnableGroup_10ms_offset3(void)
{
}

void RunnableGroup_10ms_offset4(void)
{
}

void RunnableGroup_10ms_offset5(void)
{
}

void RunnableGroup_10ms_offset6(void)
{
}

void RunnableGroup_10ms_offset7(void)
{
}

void RunnableGroup_10ms_offset8(void)
{
}

void RunnableGroup_10ms_offset9(void)
{
}

void RunnableGroup_20ms_offset0(void)
{
    DriveTrain_Run_Update();
    MotorPortHandler_Run_Update();
    RingLedDisplay_Run_Update();
    LEDController_Run_Update();
}

void RunnableGroup_20ms_offset1(void)
{
    SensorPortHandler_Run_Update();
}

void RunnableGroup_20ms_offset2(void)
{
}

void RunnableGroup_20ms_offset3(void)
{
}

void RunnableGroup_20ms_offset4(void)
{
}

void RunnableGroup_20ms_offset5(void)
{
}

void RunnableGroup_20ms_offset6(void)
{
}

void RunnableGroup_20ms_offset7(void)
{
}

void RunnableGroup_20ms_offset8(void)
{
}

void RunnableGroup_20ms_offset9(void)
{
}

void RunnableGroup_20ms_offset10(void)
{
}

void RunnableGroup_20ms_offset11(void)
{
}

void RunnableGroup_20ms_offset12(void)
{
}

void RunnableGroup_20ms_offset13(void)
{
}

void RunnableGroup_20ms_offset14(void)
{
}

void RunnableGroup_20ms_offset15(void)
{
}

void RunnableGroup_20ms_offset16(void)
{
}

void RunnableGroup_20ms_offset17(void)
{
}

void RunnableGroup_20ms_offset18(void)
{
}

void RunnableGroup_20ms_offset19(void)
{
}

void RunnableGroup_100ms(void)
{
    BatteryCalculator_Run_Update();
    BluetoothIndicator_Run_Update();
    BrainStatusIndicator_Run_Update();
}

float YawAngleTracker_Read_SampleTime(void)
{
    return IMU_Constant_SampleTime();
}

void BatteryCharger_Write_ChargerState(ChargerState_t value)
{
    BatteryCharger_ChargerState_databuffer = value;
}

ChargerState_t BatteryStatusProvider_Read_IsMainBatteryCharging(void)
{
    return BatteryCharger_ChargerState_databuffer;
}

void CommunicationObserver_Write_Enabled(bool value)
{
    CommunicationObserver_Enabled_databuffer = value;
}

bool RingLedDisplay_Read_MasterReady(void)
{
    return CommunicationObserver_Enabled_databuffer;
}

void BluetoothStatusObserver_Write_ConnectionStatus(BluetoothStatus_t value)
{
    BluetoothStatusObserver_ConnectionStatus_databuffer = value;
}

BluetoothStatus_t BluetoothIndicator_Read_ConnectionStatus(void)
{
    return BluetoothStatusObserver_ConnectionStatus_databuffer;
}

void IMU_Write_GyroscopeSample(const Vector3D_t* value)
{
    IMU_GyroscopeSample_databuffer = *value;
}

void GyroscopeOffsetCompensator_Read_AngularSpeeds(Vector3D_t* value)
{
    *value = IMU_GyroscopeSample_databuffer;
}

void IMU_Write_GyroscopeDataReady(bool value)
{
    IMU_GyroscopeDataReady_databuffer = value;
}

bool GyroscopeOffsetCompensator_Read_DataReady(void)
{
    return IMU_GyroscopeDataReady_databuffer;
}

void ADC0_Write_RawChannelData(uint32_t index, uint16_t value)
{
    ASSERT(index < ARRAY_SIZE(ADC0_RawChannelData_databuffer));
    ADC0_RawChannelData_databuffer[index] = value;
}

uint16_t ADCDispatcher_Read_ADC0_RawChannelData(uint32_t index)
{
    ASSERT(index < ARRAY_SIZE(ADC0_RawChannelData_databuffer));
    return ADC0_RawChannelData_databuffer[index];
}

void ADC1_Write_RawChannelData(uint32_t index, uint16_t value)
{
    ASSERT(index < ARRAY_SIZE(ADC1_RawChannelData_databuffer));
    ADC1_RawChannelData_databuffer[index] = value;
}

uint16_t ADCDispatcher_Read_ADC1_RawChannelData(uint32_t index)
{
    ASSERT(index < ARRAY_SIZE(ADC1_RawChannelData_databuffer));
    return ADC1_RawChannelData_databuffer[index];
}

void ADC0_Write_ChannelVoltage(uint32_t index, Voltage_t value)
{
    ASSERT(index < ARRAY_SIZE(ADC0_ChannelVoltage_databuffer));
    ADC0_ChannelVoltage_databuffer[index] = value;
}

Voltage_t ADCDispatcher_Read_ADC0_ChannelVoltage(uint32_t index)
{
    ASSERT(index < ARRAY_SIZE(ADC0_ChannelVoltage_databuffer));
    return ADC0_ChannelVoltage_databuffer[index];
}

void ADC1_Write_ChannelVoltage(uint32_t index, Voltage_t value)
{
    ASSERT(index < ARRAY_SIZE(ADC1_ChannelVoltage_databuffer));
    ADC1_ChannelVoltage_databuffer[index] = value;
}

Voltage_t ADCDispatcher_Read_ADC1_ChannelVoltage(uint32_t index)
{
    ASSERT(index < ARRAY_SIZE(ADC1_ChannelVoltage_databuffer));
    return ADC1_ChannelVoltage_databuffer[index];
}

void ADCDispatcher_Write_MotorCurrent(uint32_t index, Current_t value)
{
    ASSERT(index < ARRAY_SIZE(ADCDispatcher_MotorCurrent_databuffer));
    ADCDispatcher_MotorCurrent_databuffer[index] = value;
}

Current_t MotorCurrentFilter_Read_RawCurrent(uint32_t index)
{
    ASSERT(index < ARRAY_SIZE(ADCDispatcher_MotorCurrent_databuffer));
    return ADCDispatcher_MotorCurrent_databuffer[index];
}

void ADCDispatcher_Write_MainBatteryVoltage(Voltage_t value)
{
    ADCDispatcher_MainBatteryVoltage_databuffer = value;
}

Voltage_t BatteryCalculator_Read_MainBatteryVoltage(void)
{
    return ADCDispatcher_MainBatteryVoltage_databuffer;
}

void ADCDispatcher_Write_MotorBatteryVoltage(Voltage_t value)
{
    ADCDispatcher_MotorBatteryVoltage_databuffer = value;
}

Voltage_t BatteryCalculator_Read_MotorBatteryVoltage(void)
{
    return ADCDispatcher_MotorBatteryVoltage_databuffer;
}

void ADCDispatcher_Write_Sensor_ADC(uint32_t index, uint8_t value)
{
    ASSERT(index < ARRAY_SIZE(ADCDispatcher_Sensor_ADC_databuffer));
    ADCDispatcher_Sensor_ADC_databuffer[index] = value;
}

uint8_t SensorPortHandler_Read_AdcData(uint32_t index)
{
    ASSERT(index < ARRAY_SIZE(ADCDispatcher_Sensor_ADC_databuffer));
    return ADCDispatcher_Sensor_ADC_databuffer[index];
}

void MotorCurrentFilter_Write_FilteredCurrent(uint32_t index, Current_t value)
{
    ASSERT(index < ARRAY_SIZE(MotorCurrentFilter_FilteredCurrent_databuffer));
    MotorCurrentFilter_FilteredCurrent_databuffer[index] = value;
}

void BatteryCalculator_Read_MainBatteryParameters(BatteryConfiguration_t* value)
{
    ProjectConfiguration_Constant_MainBatteryParameters(value);
}

void BatteryCalculator_Read_MotorBatteryParameters(BatteryConfiguration_t* value)
{
    ProjectConfiguration_Constant_MotorBatteryParameters(value);
}

void CommunicationObserver_Call_ErrorLimitReached(void)
{
    RestartManager_Run_Reset();
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

void MasterCommunicationInterface_Call_RxTimeout(void)
{
    CommunicationObserver_Run_OnMessageMissed();
}

uint16_t SensorPortHandler_Call_ReadCurrentTicks(void)
{
    return HighResolutionTimer_Run_GetTickCount();
}

