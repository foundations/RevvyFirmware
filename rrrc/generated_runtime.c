#include "generated_runtime.h"

static ChargerState_t BatteryCharger_ChargerState_databuffer = ChargerState_NotPluggedIn;
static bool CommunicationObserver_Enabled_databuffer = false;
static BluetoothStatus_t BluetoothStatusObserver_ConnectionStatus_databuffer = BluetoothStatus_Inactive;

void RunnableGroup_OnInit(void)
{
    ErrorStorage_Run_OnInit();
    ADC0_Run_OnInit();
    ADC1_Run_OnInit();
    BatteryCharger_Run_OnInit();
    LEDController_Run_OnInit();
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

