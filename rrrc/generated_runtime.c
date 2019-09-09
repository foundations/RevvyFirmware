#include "generated_runtime.h"


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

void CommunicationObserver_Call_ErrorLimitReached(void)
{
    RestartManager_Run_Reset();
}

