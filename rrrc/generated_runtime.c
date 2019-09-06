#include "generated_runtime.h"

void RunnableGroup_OnInit(void)
{
    ErrorStorage_Run_OnInit();
    ADC_Run_OnInit();
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
}
