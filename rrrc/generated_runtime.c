#include "generated_runtime.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */

/* End User Code Section: Declarations */
static bool CommunicationObserver_Enabled_variable = false;
static Vector3D_t IMU_GyroscopeSample_queue;
static bool IMU_GyroscopeSample_queue_overflow = false;
static bool IMU_GyroscopeSample_queue_data_valid = false;
static Vector3D_t IMU_GyroscopeSample_queue1;
static bool IMU_GyroscopeSample_queue1_overflow = false;
static bool IMU_GyroscopeSample_queue1_data_valid = false;
static Vector3D_t GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue[10u];
static size_t GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_count = 0u;
static size_t GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_write_index = 0u;
static bool GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_overflow = false;
static bool IMUMovementDetector_IsMoving_variable = true;
static Vector3D_t IMU_AccelerometerSample_queue[10u];
static size_t IMU_AccelerometerSample_queue_count = 0u;
static size_t IMU_AccelerometerSample_queue_write_index = 0u;
static bool IMU_AccelerometerSample_queue_overflow = false;
static Orientation3D_t IMUOrientationEstimator_OrientationEulerDegrees_variable = { .pitch = 0.0f, .roll = 0.0f, .yaw = 0.0f };
static uint16_t ADC0_RawChannelData_array[4] = { 0u, 0u, 0u, 0u };
static uint16_t ADC1_RawChannelData_array[8] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u };
static Voltage_t ADC0_ChannelVoltage_array[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
static Voltage_t ADC1_ChannelVoltage_array[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static Current_t ADCDispatcher_MotorCurrent_array[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static Voltage_t ADCDispatcher_MainBatteryVoltage_variable = 0.0f;
static Voltage_t ADCDispatcher_MotorBatteryVoltage_variable = 0.0f;
static uint8_t ADCDispatcher_Sensor_ADC_array[4] = { 0u, 0u, 0u, 0u };
static Current_t MotorCurrentFilter_FilteredCurrent_array[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static Temperature_t MotorThermalModel_Temperature_array[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static BluetoothStatus_t BluetoothStatusObserver_ConnectionStatus_variable = BluetoothStatus_Inactive;
static uint8_t BatteryCalculator_MainBatteryLevel_variable = 0u;
static bool BatteryCalculator_MainBatteryLow_variable = false;
static ChargerState_t BatteryCharger_ChargerState_variable = ChargerState_NotPluggedIn;
static uint8_t BatteryCalculator_MotorBatteryLevel_variable = 0u;
static bool BatteryCalculator_MotorBatteryPresent_variable = false;
static rgb_t LedDisplayController_Leds_array[16] = { (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0} };
static uint8_t LedDisplayController_MaxBrightness_variable = 0u;
static rgb_t RingLedDisplay_LedColor_array[12] = { (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0} };
static MasterStatus_t MasterStatusObserver_MasterStatus_variable = MasterStatus_Unknown;
static int8_t MotorPortHandler_DriveStrength_array[6] = { 0, 0, 0, 0, 0, 0 };
static bool DriveTrain_MotorUsed_array[6] = { false, false, false, false, false, false };
static DriveRequest_t DriveTrain_DriveRequest_array[6] = { { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } } };
static DriveRequest_t MotorPortHandler_DriveRequest_array[6] = { { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } } };
static DriveRequest_t DriveRequestMultiplexer_AppliedDriveRequest_array[6] = { { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } }, { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = { .power = 0 } } };
static IMU_RawSample_t IMU_RawAccelerometerSample_variable = { .x = 0, .y = 0, .z = 0 };
static IMU_RawSample_t IMU_RawGyroscopeSample_variable = { .x = 0, .y = 0, .z = 0 };
static SlotData_t McuStatusSlots_SlotData_array[14] = { { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu }, { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu } };
static rgb_t CommWrapper_LedDisplay_UserFrame_array[12] = { (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0}, (rgb_t){0, 0, 0} };
static RingLedScenario_t CommWrapper_LedDisplay_Scenario_variable = RingLedScenario_BusyIndicator;
static bool StartupReasonProvider_IsColdStart_variable = false;

void Runtime_Call_OnInit(void)
{
    /* Begin User Code Section: Runtime/OnInit Start */

    /* End User Code Section: Runtime/OnInit Start */
    HardwareCompatibilityChecker_Run_OnInit();
    StartupReasonProvider_Run_OnInit();
    ErrorStorage_Run_OnInit();
    ADC0_Run_OnInit();
    ADC1_Run_OnInit();
    MotorCurrentFilter_Run_OnInit();
    BatteryCharger_Run_OnInit();
    LEDController_Run_OnInit();
    BatteryCalculator_Run_OnInit();
    IMU_Run_OnInit();
    HighResolutionTimer_Run_OnInit();
    MasterStatusObserver_Run_OnInit();
    MotorThermalModel_Run_OnInit();
    BluetoothStatusObserver_Run_OnInit();
    RingLedDisplay_Run_OnInit();
    CommunicationObserver_Run_OnInit();
    DriveTrain_Run_OnInit();
    GyroscopeOffsetCompensator_Run_OnInit();
    MasterCommunicationInterface_Run_OnInit();
    LedDisplayController_Run_OnInit();
    IMUMovementDetector_Run_OnInit();
    IMUOrientationEstimator_Run_OnInit();
    McuStatusSlots_Run_Reset();
    McuStatusCollector_Run_Reset();
    MotorDriver_8833_Run_OnInit();
    /* Begin User Code Section: Runtime/OnInit End */

    /* End User Code Section: Runtime/OnInit End */
}

void Runtime_Call_1ms(void)
{
    /* Begin User Code Section: Runtime/1ms Start */

    /* End User Code Section: Runtime/1ms Start */
    ADC0_Run_Update();
    ADC1_Run_Update();
    IMU_Run_OnUpdate();
    IMUMovementDetector_Run_OnUpdate();
    GyroscopeOffsetCompensator_Run_Update();
    ADCDispatcher_Run_Update();
    MotorCurrentFilter_Run_Update();
    MotorThermalModel_Run_OnUpdate();
    McuStatusSlots_Run_Update();
    /* Begin User Code Section: Runtime/1ms End */

    /* End User Code Section: Runtime/1ms End */
}

void Runtime_Call_10ms_offset0(void)
{
    /* Begin User Code Section: Runtime/10ms_offset0 Start */

    /* End User Code Section: Runtime/10ms_offset0 Start */
    BatteryCharger_Run_Update();
    WatchdogFeeder_Run_Feed();
    MotorDerating_Run_OnUpdate();
    DriveTrain_Run_Update();
    IMUOrientationEstimator_Run_OnUpdate();
    DriveRequestMultiplexer_Run_OnUpdate();
    /* Begin User Code Section: Runtime/10ms_offset0 End */

    /* End User Code Section: Runtime/10ms_offset0 End */
}

void Runtime_Call_10ms_offset1(void)
{
    /* Begin User Code Section: Runtime/10ms_offset1 Start */

    /* End User Code Section: Runtime/10ms_offset1 Start */
    /* Begin User Code Section: Runtime/10ms_offset1 End */

    /* End User Code Section: Runtime/10ms_offset1 End */
}

void Runtime_Call_10ms_offset2(void)
{
    /* Begin User Code Section: Runtime/10ms_offset2 Start */

    /* End User Code Section: Runtime/10ms_offset2 Start */
    MotorPortHandler_Run_PortUpdate(4);
    MotorPortHandler_Run_PortUpdate(5);
    /* Begin User Code Section: Runtime/10ms_offset2 End */

    /* End User Code Section: Runtime/10ms_offset2 End */
}

void Runtime_Call_10ms_offset3(void)
{
    /* Begin User Code Section: Runtime/10ms_offset3 Start */

    /* End User Code Section: Runtime/10ms_offset3 Start */
    MotorPortHandler_Run_PortUpdate(1);
    MotorPortHandler_Run_PortUpdate(2);
    /* Begin User Code Section: Runtime/10ms_offset3 End */

    /* End User Code Section: Runtime/10ms_offset3 End */
}

void Runtime_Call_10ms_offset4(void)
{
    /* Begin User Code Section: Runtime/10ms_offset4 Start */

    /* End User Code Section: Runtime/10ms_offset4 Start */
    MotorPortHandler_Run_PortUpdate(0);
    MotorPortHandler_Run_PortUpdate(3);
    /* Begin User Code Section: Runtime/10ms_offset4 End */

    /* End User Code Section: Runtime/10ms_offset4 End */
}

void Runtime_Call_10ms_offset5(void)
{
    /* Begin User Code Section: Runtime/10ms_offset5 Start */

    /* End User Code Section: Runtime/10ms_offset5 Start */
    /* Begin User Code Section: Runtime/10ms_offset5 End */

    /* End User Code Section: Runtime/10ms_offset5 End */
}

void Runtime_Call_10ms_offset6(void)
{
    /* Begin User Code Section: Runtime/10ms_offset6 Start */

    /* End User Code Section: Runtime/10ms_offset6 Start */
    /* Begin User Code Section: Runtime/10ms_offset6 End */

    /* End User Code Section: Runtime/10ms_offset6 End */
}

void Runtime_Call_10ms_offset7(void)
{
    /* Begin User Code Section: Runtime/10ms_offset7 Start */

    /* End User Code Section: Runtime/10ms_offset7 Start */
    /* Begin User Code Section: Runtime/10ms_offset7 End */

    /* End User Code Section: Runtime/10ms_offset7 End */
}

void Runtime_Call_10ms_offset8(void)
{
    /* Begin User Code Section: Runtime/10ms_offset8 Start */

    /* End User Code Section: Runtime/10ms_offset8 Start */
    /* Begin User Code Section: Runtime/10ms_offset8 End */

    /* End User Code Section: Runtime/10ms_offset8 End */
}

void Runtime_Call_10ms_offset9(void)
{
    /* Begin User Code Section: Runtime/10ms_offset9 Start */

    /* End User Code Section: Runtime/10ms_offset9 Start */
    /* Begin User Code Section: Runtime/10ms_offset9 End */

    /* End User Code Section: Runtime/10ms_offset9 End */
}

void Runtime_Call_20ms_offset0(void)
{
    /* Begin User Code Section: Runtime/20ms_offset0 Start */

    /* End User Code Section: Runtime/20ms_offset0 Start */
    RingLedDisplay_Run_Update();
    /* Begin User Code Section: Runtime/20ms_offset0 End */

    /* End User Code Section: Runtime/20ms_offset0 End */
}

void Runtime_Call_20ms_offset1(void)
{
    /* Begin User Code Section: Runtime/20ms_offset1 Start */

    /* End User Code Section: Runtime/20ms_offset1 Start */
    LedDisplayController_Run_Update();
    LEDController_Run_Update();
    /* Begin User Code Section: Runtime/20ms_offset1 End */

    /* End User Code Section: Runtime/20ms_offset1 End */
}

void Runtime_Call_20ms_offset2(void)
{
    /* Begin User Code Section: Runtime/20ms_offset2 Start */

    /* End User Code Section: Runtime/20ms_offset2 Start */
    /* Begin User Code Section: Runtime/20ms_offset2 End */

    /* End User Code Section: Runtime/20ms_offset2 End */
}

void Runtime_Call_20ms_offset3(void)
{
    /* Begin User Code Section: Runtime/20ms_offset3 Start */

    /* End User Code Section: Runtime/20ms_offset3 Start */
    /* Begin User Code Section: Runtime/20ms_offset3 End */

    /* End User Code Section: Runtime/20ms_offset3 End */
}

void Runtime_Call_20ms_offset4(void)
{
    /* Begin User Code Section: Runtime/20ms_offset4 Start */

    /* End User Code Section: Runtime/20ms_offset4 Start */
    /* Begin User Code Section: Runtime/20ms_offset4 End */

    /* End User Code Section: Runtime/20ms_offset4 End */
}

void Runtime_Call_20ms_offset5(void)
{
    /* Begin User Code Section: Runtime/20ms_offset5 Start */

    /* End User Code Section: Runtime/20ms_offset5 Start */
    SensorPortHandler_Run_PortUpdate(0);
    /* Begin User Code Section: Runtime/20ms_offset5 End */

    /* End User Code Section: Runtime/20ms_offset5 End */
}

void Runtime_Call_20ms_offset6(void)
{
    /* Begin User Code Section: Runtime/20ms_offset6 Start */

    /* End User Code Section: Runtime/20ms_offset6 Start */
    SensorPortHandler_Run_PortUpdate(1);
    /* Begin User Code Section: Runtime/20ms_offset6 End */

    /* End User Code Section: Runtime/20ms_offset6 End */
}

void Runtime_Call_20ms_offset7(void)
{
    /* Begin User Code Section: Runtime/20ms_offset7 Start */

    /* End User Code Section: Runtime/20ms_offset7 Start */
    SensorPortHandler_Run_PortUpdate(2);
    /* Begin User Code Section: Runtime/20ms_offset7 End */

    /* End User Code Section: Runtime/20ms_offset7 End */
}

void Runtime_Call_20ms_offset8(void)
{
    /* Begin User Code Section: Runtime/20ms_offset8 Start */

    /* End User Code Section: Runtime/20ms_offset8 Start */
    SensorPortHandler_Run_PortUpdate(3);
    /* Begin User Code Section: Runtime/20ms_offset8 End */

    /* End User Code Section: Runtime/20ms_offset8 End */
}

void Runtime_Call_20ms_offset9(void)
{
    /* Begin User Code Section: Runtime/20ms_offset9 Start */

    /* End User Code Section: Runtime/20ms_offset9 Start */
    /* Begin User Code Section: Runtime/20ms_offset9 End */

    /* End User Code Section: Runtime/20ms_offset9 End */
}

void Runtime_Call_20ms_offset10(void)
{
    /* Begin User Code Section: Runtime/20ms_offset10 Start */

    /* End User Code Section: Runtime/20ms_offset10 Start */
    /* Begin User Code Section: Runtime/20ms_offset10 End */

    /* End User Code Section: Runtime/20ms_offset10 End */
}

void Runtime_Call_20ms_offset11(void)
{
    /* Begin User Code Section: Runtime/20ms_offset11 Start */

    /* End User Code Section: Runtime/20ms_offset11 Start */
    /* Begin User Code Section: Runtime/20ms_offset11 End */

    /* End User Code Section: Runtime/20ms_offset11 End */
}

void Runtime_Call_20ms_offset12(void)
{
    /* Begin User Code Section: Runtime/20ms_offset12 Start */

    /* End User Code Section: Runtime/20ms_offset12 Start */
    /* Begin User Code Section: Runtime/20ms_offset12 End */

    /* End User Code Section: Runtime/20ms_offset12 End */
}

void Runtime_Call_20ms_offset13(void)
{
    /* Begin User Code Section: Runtime/20ms_offset13 Start */

    /* End User Code Section: Runtime/20ms_offset13 Start */
    /* Begin User Code Section: Runtime/20ms_offset13 End */

    /* End User Code Section: Runtime/20ms_offset13 End */
}

void Runtime_Call_20ms_offset14(void)
{
    /* Begin User Code Section: Runtime/20ms_offset14 Start */

    /* End User Code Section: Runtime/20ms_offset14 Start */
    /* Begin User Code Section: Runtime/20ms_offset14 End */

    /* End User Code Section: Runtime/20ms_offset14 End */
}

void Runtime_Call_20ms_offset15(void)
{
    /* Begin User Code Section: Runtime/20ms_offset15 Start */

    /* End User Code Section: Runtime/20ms_offset15 Start */
    /* Begin User Code Section: Runtime/20ms_offset15 End */

    /* End User Code Section: Runtime/20ms_offset15 End */
}

void Runtime_Call_20ms_offset16(void)
{
    /* Begin User Code Section: Runtime/20ms_offset16 Start */

    /* End User Code Section: Runtime/20ms_offset16 Start */
    /* Begin User Code Section: Runtime/20ms_offset16 End */

    /* End User Code Section: Runtime/20ms_offset16 End */
}

void Runtime_Call_20ms_offset17(void)
{
    /* Begin User Code Section: Runtime/20ms_offset17 Start */

    /* End User Code Section: Runtime/20ms_offset17 Start */
    /* Begin User Code Section: Runtime/20ms_offset17 End */

    /* End User Code Section: Runtime/20ms_offset17 End */
}

void Runtime_Call_20ms_offset18(void)
{
    /* Begin User Code Section: Runtime/20ms_offset18 Start */

    /* End User Code Section: Runtime/20ms_offset18 Start */
    /* Begin User Code Section: Runtime/20ms_offset18 End */

    /* End User Code Section: Runtime/20ms_offset18 End */
}

void Runtime_Call_20ms_offset19(void)
{
    /* Begin User Code Section: Runtime/20ms_offset19 Start */

    /* End User Code Section: Runtime/20ms_offset19 Start */
    /* Begin User Code Section: Runtime/20ms_offset19 End */

    /* End User Code Section: Runtime/20ms_offset19 End */
}

void Runtime_Call_100ms(void)
{
    /* Begin User Code Section: Runtime/100ms Start */

    /* End User Code Section: Runtime/100ms Start */
    SensorPortHandler_Run_Update();
    MotorPortHandler_Run_Update();
    BatteryCalculator_Run_Update();
    /* Begin User Code Section: Runtime/100ms End */

    /* End User Code Section: Runtime/100ms End */
}

void HardwareCompatibilityChecker_Call_OnIncompatibleHardware(void)
{
    /* Begin User Code Section: HardwareCompatibilityChecker/OnIncompatibleHardware Start */

    /* End User Code Section: HardwareCompatibilityChecker/OnIncompatibleHardware Start */
    RestartManager_Run_RebootToBootloader();
    /* Begin User Code Section: HardwareCompatibilityChecker/OnIncompatibleHardware End */

    /* End User Code Section: HardwareCompatibilityChecker/OnIncompatibleHardware End */
}

void CommunicationObserver_Call_ErrorLimitReached(void)
{
    /* Begin User Code Section: CommunicationObserver/ErrorLimitReached Start */

    /* End User Code Section: CommunicationObserver/ErrorLimitReached Start */
    RestartManager_Run_Reset();
    /* Begin User Code Section: CommunicationObserver/ErrorLimitReached End */

    /* End User Code Section: CommunicationObserver/ErrorLimitReached End */
}

void MasterCommunicationInterface_Call_RxTimeout(void)
{
    /* Begin User Code Section: MasterCommunicationInterface/RxTimeout Start */

    /* End User Code Section: MasterCommunicationInterface/RxTimeout Start */
    CommunicationObserver_Run_OnMessageMissed();
    /* Begin User Code Section: MasterCommunicationInterface/RxTimeout End */

    /* End User Code Section: MasterCommunicationInterface/RxTimeout End */
}

void* SensorPortHandler_Call_Allocate(size_t size)
{
    /* Begin User Code Section: SensorPortHandler/Allocate Start */

    /* End User Code Section: SensorPortHandler/Allocate Start */
    void* return_value = MemoryAllocator_Run_Allocate(size);
    /* Begin User Code Section: SensorPortHandler/Allocate End */

    /* End User Code Section: SensorPortHandler/Allocate End */
    return return_value;
}

void SensorPortHandler_Call_Free(void** ptr)
{
    /* Begin User Code Section: SensorPortHandler/Free Start */

    /* End User Code Section: SensorPortHandler/Free Start */
    MemoryAllocator_Run_Free(ptr);
    /* Begin User Code Section: SensorPortHandler/Free End */

    /* End User Code Section: SensorPortHandler/Free End */
}

void* MotorPortHandler_Call_Allocate(size_t size)
{
    /* Begin User Code Section: MotorPortHandler/Allocate Start */

    /* End User Code Section: MotorPortHandler/Allocate Start */
    void* return_value = MemoryAllocator_Run_Allocate(size);
    /* Begin User Code Section: MotorPortHandler/Allocate End */

    /* End User Code Section: MotorPortHandler/Allocate End */
    return return_value;
}

void MotorPortHandler_Call_Free(void** ptr)
{
    /* Begin User Code Section: MotorPortHandler/Free Start */

    /* End User Code Section: MotorPortHandler/Free Start */
    MemoryAllocator_Run_Free(ptr);
    /* Begin User Code Section: MotorPortHandler/Free End */

    /* End User Code Section: MotorPortHandler/Free End */
}

uint16_t SensorPortHandler_Call_ReadCurrentTicks(void)
{
    /* Begin User Code Section: SensorPortHandler/ReadCurrentTicks Start */

    /* End User Code Section: SensorPortHandler/ReadCurrentTicks Start */
    uint16_t return_value = HighResolutionTimer_Run_GetTickCount();
    /* Begin User Code Section: SensorPortHandler/ReadCurrentTicks End */

    /* End User Code Section: SensorPortHandler/ReadCurrentTicks End */
    return return_value;
}

float SensorPortHandler_Call_ConvertTicksToMs(uint32_t ticks)
{
    /* Begin User Code Section: SensorPortHandler/ConvertTicksToMs Start */

    /* End User Code Section: SensorPortHandler/ConvertTicksToMs Start */
    float return_value = HighResolutionTimer_Run_ToMs(ticks);
    /* Begin User Code Section: SensorPortHandler/ConvertTicksToMs End */

    /* End User Code Section: SensorPortHandler/ConvertTicksToMs End */
    return return_value;
}

void McuStatusCollectorWrapper_Call_ResetSlots(void)
{
    /* Begin User Code Section: McuStatusCollectorWrapper/ResetSlots Start */

    /* End User Code Section: McuStatusCollectorWrapper/ResetSlots Start */
    McuStatusCollector_Run_Reset();
    /* Begin User Code Section: McuStatusCollectorWrapper/ResetSlots End */

    /* End User Code Section: McuStatusCollectorWrapper/ResetSlots End */
}

void McuStatusCollectorWrapper_Call_EnableSlot(uint8_t slot)
{
    /* Begin User Code Section: McuStatusCollectorWrapper/EnableSlot Start */

    /* End User Code Section: McuStatusCollectorWrapper/EnableSlot Start */
    McuStatusCollector_Run_EnableSlot(slot);
    /* Begin User Code Section: McuStatusCollectorWrapper/EnableSlot End */

    /* End User Code Section: McuStatusCollectorWrapper/EnableSlot End */
}

void McuStatusCollectorWrapper_Call_DisableSlot(uint8_t slot)
{
    /* Begin User Code Section: McuStatusCollectorWrapper/DisableSlot Start */

    /* End User Code Section: McuStatusCollectorWrapper/DisableSlot Start */
    McuStatusCollector_Run_DisableSlot(slot);
    /* Begin User Code Section: McuStatusCollectorWrapper/DisableSlot End */

    /* End User Code Section: McuStatusCollectorWrapper/DisableSlot End */
}

uint8_t McuStatusCollectorWrapper_Call_Read(ByteArray_t destination)
{
    /* Begin User Code Section: McuStatusCollectorWrapper/Read Start */

    /* End User Code Section: McuStatusCollectorWrapper/Read Start */
    uint8_t return_value = McuStatusCollector_Run_Read(destination);
    /* Begin User Code Section: McuStatusCollectorWrapper/Read End */

    /* End User Code Section: McuStatusCollectorWrapper/Read End */
    return return_value;
}

void MotorPortHandler_Call_UpdatePortStatus(uint8_t port, ByteArray_t data)
{
    /* Begin User Code Section: MotorPortHandler/UpdatePortStatus Start */

    /* End User Code Section: MotorPortHandler/UpdatePortStatus Start */
    McuStatusSlots_Run_UpdateMotorPort(port, data);
    /* Begin User Code Section: MotorPortHandler/UpdatePortStatus End */

    /* End User Code Section: MotorPortHandler/UpdatePortStatus End */
}

void SensorPortHandler_Call_UpdatePortStatus(uint8_t port, ByteArray_t data)
{
    /* Begin User Code Section: SensorPortHandler/UpdatePortStatus Start */

    /* End User Code Section: SensorPortHandler/UpdatePortStatus Start */
    McuStatusSlots_Run_UpdateSensorPort(port, data);
    /* Begin User Code Section: SensorPortHandler/UpdatePortStatus End */

    /* End User Code Section: SensorPortHandler/UpdatePortStatus End */
}

uint8_t CommWrapper_LedDisplay_Call_ReadScenarioName(RingLedScenario_t scenario, ByteArray_t destination)
{
    /* Begin User Code Section: CommWrapper_LedDisplay/ReadScenarioName Start */

    /* End User Code Section: CommWrapper_LedDisplay/ReadScenarioName Start */
    uint8_t return_value = RingLedDisplay_Run_ReadScenarioName(scenario, destination);
    /* Begin User Code Section: CommWrapper_LedDisplay/ReadScenarioName End */

    /* End User Code Section: CommWrapper_LedDisplay/ReadScenarioName End */
    return return_value;
}

void ADC0_Write_ChannelVoltage(uint32_t index, const Voltage_t value)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADC0/ChannelVoltage Start */

    /* End User Code Section: ADC0/ChannelVoltage Start */
    ADC0_ChannelVoltage_array[index] = value;
    /* Begin User Code Section: ADC0/ChannelVoltage End */

    /* End User Code Section: ADC0/ChannelVoltage End */
}

void ADC0_Write_RawChannelData(uint32_t index, const uint16_t value)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADC0/RawChannelData Start */

    /* End User Code Section: ADC0/RawChannelData Start */
    ADC0_RawChannelData_array[index] = value;
    /* Begin User Code Section: ADC0/RawChannelData End */

    /* End User Code Section: ADC0/RawChannelData End */
}

void ADC1_Write_ChannelVoltage(uint32_t index, const Voltage_t value)
{
    ASSERT(index < 8);
    /* Begin User Code Section: ADC1/ChannelVoltage Start */

    /* End User Code Section: ADC1/ChannelVoltage Start */
    ADC1_ChannelVoltage_array[index] = value;
    /* Begin User Code Section: ADC1/ChannelVoltage End */

    /* End User Code Section: ADC1/ChannelVoltage End */
}

void ADC1_Write_RawChannelData(uint32_t index, const uint16_t value)
{
    ASSERT(index < 8);
    /* Begin User Code Section: ADC1/RawChannelData Start */

    /* End User Code Section: ADC1/RawChannelData Start */
    ADC1_RawChannelData_array[index] = value;
    /* Begin User Code Section: ADC1/RawChannelData End */

    /* End User Code Section: ADC1/RawChannelData End */
}

void ADCDispatcher_Write_MainBatteryVoltage(const Voltage_t value)
{
    /* Begin User Code Section: ADCDispatcher/MainBatteryVoltage Start */

    /* End User Code Section: ADCDispatcher/MainBatteryVoltage Start */
    ADCDispatcher_MainBatteryVoltage_variable = value;
    /* Begin User Code Section: ADCDispatcher/MainBatteryVoltage End */

    /* End User Code Section: ADCDispatcher/MainBatteryVoltage End */
}

void ADCDispatcher_Write_MotorBatteryVoltage(const Voltage_t value)
{
    /* Begin User Code Section: ADCDispatcher/MotorBatteryVoltage Start */

    /* End User Code Section: ADCDispatcher/MotorBatteryVoltage Start */
    ADCDispatcher_MotorBatteryVoltage_variable = value;
    /* Begin User Code Section: ADCDispatcher/MotorBatteryVoltage End */

    /* End User Code Section: ADCDispatcher/MotorBatteryVoltage End */
}

void ADCDispatcher_Write_MotorCurrent(uint32_t index, const Current_t value)
{
    ASSERT(index < 6);
    /* Begin User Code Section: ADCDispatcher/MotorCurrent Start */

    /* End User Code Section: ADCDispatcher/MotorCurrent Start */
    ADCDispatcher_MotorCurrent_array[index] = value;
    /* Begin User Code Section: ADCDispatcher/MotorCurrent End */

    /* End User Code Section: ADCDispatcher/MotorCurrent End */
}

void ADCDispatcher_Write_Sensor_ADC(uint32_t index, const uint8_t value)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADCDispatcher/Sensor_ADC Start */

    /* End User Code Section: ADCDispatcher/Sensor_ADC Start */
    ADCDispatcher_Sensor_ADC_array[index] = value;
    /* Begin User Code Section: ADCDispatcher/Sensor_ADC End */

    /* End User Code Section: ADCDispatcher/Sensor_ADC End */
}

void BatteryCalculator_Write_MainBatteryLevel(const uint8_t value)
{
    /* Begin User Code Section: BatteryCalculator/MainBatteryLevel Start */

    /* End User Code Section: BatteryCalculator/MainBatteryLevel Start */
    BatteryCalculator_MainBatteryLevel_variable = value;
    /* Begin User Code Section: BatteryCalculator/MainBatteryLevel End */

    /* End User Code Section: BatteryCalculator/MainBatteryLevel End */
}

void BatteryCalculator_Write_MainBatteryLow(const bool value)
{
    /* Begin User Code Section: BatteryCalculator/MainBatteryLow Start */

    /* End User Code Section: BatteryCalculator/MainBatteryLow Start */
    BatteryCalculator_MainBatteryLow_variable = value;
    /* Begin User Code Section: BatteryCalculator/MainBatteryLow End */

    /* End User Code Section: BatteryCalculator/MainBatteryLow End */
}

void BatteryCalculator_Write_MotorBatteryLevel(const uint8_t value)
{
    /* Begin User Code Section: BatteryCalculator/MotorBatteryLevel Start */

    /* End User Code Section: BatteryCalculator/MotorBatteryLevel Start */
    BatteryCalculator_MotorBatteryLevel_variable = value;
    /* Begin User Code Section: BatteryCalculator/MotorBatteryLevel End */

    /* End User Code Section: BatteryCalculator/MotorBatteryLevel End */
}

void BatteryCalculator_Write_MotorBatteryPresent(const bool value)
{
    /* Begin User Code Section: BatteryCalculator/MotorBatteryPresent Start */

    /* End User Code Section: BatteryCalculator/MotorBatteryPresent Start */
    BatteryCalculator_MotorBatteryPresent_variable = value;
    /* Begin User Code Section: BatteryCalculator/MotorBatteryPresent End */

    /* End User Code Section: BatteryCalculator/MotorBatteryPresent End */
}

void BatteryCharger_Write_ChargerState(const ChargerState_t value)
{
    /* Begin User Code Section: BatteryCharger/ChargerState Start */

    /* End User Code Section: BatteryCharger/ChargerState Start */
    BatteryCharger_ChargerState_variable = value;
    /* Begin User Code Section: BatteryCharger/ChargerState End */

    /* End User Code Section: BatteryCharger/ChargerState End */
}

void BluetoothStatusObserver_Write_ConnectionStatus(const BluetoothStatus_t value)
{
    /* Begin User Code Section: BluetoothStatusObserver/ConnectionStatus Start */

    /* End User Code Section: BluetoothStatusObserver/ConnectionStatus Start */
    BluetoothStatusObserver_ConnectionStatus_variable = value;
    /* Begin User Code Section: BluetoothStatusObserver/ConnectionStatus End */

    /* End User Code Section: BluetoothStatusObserver/ConnectionStatus End */
}

void CommWrapper_LedDisplay_Write_Scenario(const RingLedScenario_t value)
{
    /* Begin User Code Section: CommWrapper_LedDisplay/Scenario Start */

    /* End User Code Section: CommWrapper_LedDisplay/Scenario Start */
    __disable_irq();
    CommWrapper_LedDisplay_Scenario_variable = value;
    __enable_irq();
    /* Begin User Code Section: CommWrapper_LedDisplay/Scenario End */

    /* End User Code Section: CommWrapper_LedDisplay/Scenario End */
}

void CommWrapper_LedDisplay_Write_UserFrame(uint32_t index, const rgb_t value)
{
    ASSERT(index < 12);
    /* Begin User Code Section: CommWrapper_LedDisplay/UserFrame Start */

    /* End User Code Section: CommWrapper_LedDisplay/UserFrame Start */
    __disable_irq();
    CommWrapper_LedDisplay_UserFrame_array[index] = value;
    __enable_irq();
    /* Begin User Code Section: CommWrapper_LedDisplay/UserFrame End */

    /* End User Code Section: CommWrapper_LedDisplay/UserFrame End */
}

void CommunicationObserver_Write_Enabled(const bool value)
{
    /* Begin User Code Section: CommunicationObserver/Enabled Start */

    /* End User Code Section: CommunicationObserver/Enabled Start */
    CommunicationObserver_Enabled_variable = value;
    /* Begin User Code Section: CommunicationObserver/Enabled End */

    /* End User Code Section: CommunicationObserver/Enabled End */
}

void DriveRequestMultiplexer_Write_AppliedDriveRequest(uint32_t index, const DriveRequest_t* value)
{
    ASSERT(index < 6);
    ASSERT(value != NULL);
    /* Begin User Code Section: DriveRequestMultiplexer/AppliedDriveRequest Start */

    /* End User Code Section: DriveRequestMultiplexer/AppliedDriveRequest Start */
    DriveRequestMultiplexer_AppliedDriveRequest_array[index] = *value;
    /* Begin User Code Section: DriveRequestMultiplexer/AppliedDriveRequest End */

    /* End User Code Section: DriveRequestMultiplexer/AppliedDriveRequest End */
}

void DriveTrain_Write_DriveRequest(uint32_t index, const DriveRequest_t* value)
{
    ASSERT(index < 6);
    ASSERT(value != NULL);
    /* Begin User Code Section: DriveTrain/DriveRequest Start */

    /* End User Code Section: DriveTrain/DriveRequest Start */
    DriveTrain_DriveRequest_array[index] = *value;
    /* Begin User Code Section: DriveTrain/DriveRequest End */

    /* End User Code Section: DriveTrain/DriveRequest End */
}

void DriveTrain_Write_MotorUsed(uint32_t index, const bool value)
{
    ASSERT(index < 6);
    /* Begin User Code Section: DriveTrain/MotorUsed Start */

    /* End User Code Section: DriveTrain/MotorUsed Start */
    DriveTrain_MotorUsed_array[index] = value;
    /* Begin User Code Section: DriveTrain/MotorUsed End */

    /* End User Code Section: DriveTrain/MotorUsed End */
}

void GyroscopeOffsetCompensator_Write_CompensatedAngularSpeeds(const Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: GyroscopeOffsetCompensator/CompensatedAngularSpeeds Start */

    /* End User Code Section: GyroscopeOffsetCompensator/CompensatedAngularSpeeds Start */
    if (GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_count < 10u)
    {
        ++GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_count;
    }
    else
    {
        GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_overflow = true;
    }
    size_t idx = GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_write_index;
    GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_write_index = (GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_write_index + 1u) % 10u;
    GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue[idx] = *value;
    /* Begin User Code Section: GyroscopeOffsetCompensator/CompensatedAngularSpeeds End */

    /* End User Code Section: GyroscopeOffsetCompensator/CompensatedAngularSpeeds End */
}

void IMU_Write_AccelerometerSample(const Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: IMU/AccelerometerSample Start */

    /* End User Code Section: IMU/AccelerometerSample Start */
    if (IMU_AccelerometerSample_queue_count < 10u)
    {
        ++IMU_AccelerometerSample_queue_count;
    }
    else
    {
        IMU_AccelerometerSample_queue_overflow = true;
    }
    size_t idx = IMU_AccelerometerSample_queue_write_index;
    IMU_AccelerometerSample_queue_write_index = (IMU_AccelerometerSample_queue_write_index + 1u) % 10u;
    IMU_AccelerometerSample_queue[idx] = *value;
    /* Begin User Code Section: IMU/AccelerometerSample End */

    /* End User Code Section: IMU/AccelerometerSample End */
}

void IMU_Write_GyroscopeSample(const Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: IMU/GyroscopeSample Start */

    /* End User Code Section: IMU/GyroscopeSample Start */
    IMU_GyroscopeSample_queue_overflow = IMU_GyroscopeSample_queue_data_valid;
    IMU_GyroscopeSample_queue = *value;
    IMU_GyroscopeSample_queue_data_valid = true;
    IMU_GyroscopeSample_queue1_overflow = IMU_GyroscopeSample_queue1_data_valid;
    IMU_GyroscopeSample_queue1 = *value;
    IMU_GyroscopeSample_queue1_data_valid = true;
    /* Begin User Code Section: IMU/GyroscopeSample End */

    /* End User Code Section: IMU/GyroscopeSample End */
}

void IMU_Write_RawAccelerometerSample(const IMU_RawSample_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: IMU/RawAccelerometerSample Start */

    /* End User Code Section: IMU/RawAccelerometerSample Start */
    IMU_RawAccelerometerSample_variable = *value;
    /* Begin User Code Section: IMU/RawAccelerometerSample End */

    /* End User Code Section: IMU/RawAccelerometerSample End */
}

void IMU_Write_RawGyroscopeSample(const IMU_RawSample_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: IMU/RawGyroscopeSample Start */

    /* End User Code Section: IMU/RawGyroscopeSample Start */
    IMU_RawGyroscopeSample_variable = *value;
    /* Begin User Code Section: IMU/RawGyroscopeSample End */

    /* End User Code Section: IMU/RawGyroscopeSample End */
}

void IMUMovementDetector_Write_IsMoving(const bool value)
{
    /* Begin User Code Section: IMUMovementDetector/IsMoving Start */

    /* End User Code Section: IMUMovementDetector/IsMoving Start */
    IMUMovementDetector_IsMoving_variable = value;
    /* Begin User Code Section: IMUMovementDetector/IsMoving End */

    /* End User Code Section: IMUMovementDetector/IsMoving End */
}

void IMUOrientationEstimator_Write_OrientationEulerDegrees(const Orientation3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: IMUOrientationEstimator/OrientationEulerDegrees Start */

    /* End User Code Section: IMUOrientationEstimator/OrientationEulerDegrees Start */
    IMUOrientationEstimator_OrientationEulerDegrees_variable = *value;
    /* Begin User Code Section: IMUOrientationEstimator/OrientationEulerDegrees End */

    /* End User Code Section: IMUOrientationEstimator/OrientationEulerDegrees End */
}

void LedDisplayController_Write_Leds(uint32_t index, const rgb_t value)
{
    ASSERT(index < 16);
    /* Begin User Code Section: LedDisplayController/Leds Start */

    /* End User Code Section: LedDisplayController/Leds Start */
    LedDisplayController_Leds_array[index] = value;
    /* Begin User Code Section: LedDisplayController/Leds End */

    /* End User Code Section: LedDisplayController/Leds End */
}

void LedDisplayController_Write_MaxBrightness(const uint8_t value)
{
    /* Begin User Code Section: LedDisplayController/MaxBrightness Start */

    /* End User Code Section: LedDisplayController/MaxBrightness Start */
    LedDisplayController_MaxBrightness_variable = value;
    /* Begin User Code Section: LedDisplayController/MaxBrightness End */

    /* End User Code Section: LedDisplayController/MaxBrightness End */
}

void MasterStatusObserver_Write_MasterStatus(const MasterStatus_t value)
{
    /* Begin User Code Section: MasterStatusObserver/MasterStatus Start */

    /* End User Code Section: MasterStatusObserver/MasterStatus Start */
    MasterStatusObserver_MasterStatus_variable = value;
    /* Begin User Code Section: MasterStatusObserver/MasterStatus End */

    /* End User Code Section: MasterStatusObserver/MasterStatus End */
}

void McuStatusSlots_Write_SlotData(uint32_t index, const SlotData_t value)
{
    ASSERT(index < 14);
    /* Begin User Code Section: McuStatusSlots/SlotData Start */

    /* End User Code Section: McuStatusSlots/SlotData Start */
    McuStatusSlots_SlotData_array[index] = value;
    /* Begin User Code Section: McuStatusSlots/SlotData End */

    /* End User Code Section: McuStatusSlots/SlotData End */
}

void MotorCurrentFilter_Write_FilteredCurrent(uint32_t index, const Current_t value)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorCurrentFilter/FilteredCurrent Start */

    /* End User Code Section: MotorCurrentFilter/FilteredCurrent Start */
    MotorCurrentFilter_FilteredCurrent_array[index] = value;
    /* Begin User Code Section: MotorCurrentFilter/FilteredCurrent End */

    /* End User Code Section: MotorCurrentFilter/FilteredCurrent End */
}

void MotorPortHandler_Write_DriveRequest(uint32_t index, const DriveRequest_t* value)
{
    ASSERT(index < 6);
    ASSERT(value != NULL);
    /* Begin User Code Section: MotorPortHandler/DriveRequest Start */

    /* End User Code Section: MotorPortHandler/DriveRequest Start */
    __disable_irq();
    MotorPortHandler_DriveRequest_array[index] = *value;
    __enable_irq();
    /* Begin User Code Section: MotorPortHandler/DriveRequest End */

    /* End User Code Section: MotorPortHandler/DriveRequest End */
}

void MotorPortHandler_Write_DriveStrength(uint32_t index, const int8_t value)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorPortHandler/DriveStrength Start */

    /* End User Code Section: MotorPortHandler/DriveStrength Start */
    MotorPortHandler_DriveStrength_array[index] = value;
    /* Begin User Code Section: MotorPortHandler/DriveStrength End */

    /* End User Code Section: MotorPortHandler/DriveStrength End */
}

void MotorThermalModel_Write_Temperature(uint32_t index, const Temperature_t value)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorThermalModel/Temperature Start */

    /* End User Code Section: MotorThermalModel/Temperature Start */
    MotorThermalModel_Temperature_array[index] = value;
    /* Begin User Code Section: MotorThermalModel/Temperature End */

    /* End User Code Section: MotorThermalModel/Temperature End */
}

void RingLedDisplay_Write_LedColor(uint32_t index, const rgb_t value)
{
    ASSERT(index < 12);
    /* Begin User Code Section: RingLedDisplay/LedColor Start */

    /* End User Code Section: RingLedDisplay/LedColor Start */
    RingLedDisplay_LedColor_array[index] = value;
    /* Begin User Code Section: RingLedDisplay/LedColor End */

    /* End User Code Section: RingLedDisplay/LedColor End */
}

void StartupReasonProvider_Write_IsColdStart(const bool value)
{
    /* Begin User Code Section: StartupReasonProvider/IsColdStart Start */

    /* End User Code Section: StartupReasonProvider/IsColdStart Start */
    StartupReasonProvider_IsColdStart_variable = value;
    /* Begin User Code Section: StartupReasonProvider/IsColdStart End */

    /* End User Code Section: StartupReasonProvider/IsColdStart End */
}

Voltage_t ADCDispatcher_Read_ADC0_ChannelVoltage(uint32_t index)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADCDispatcher/ADC0_ChannelVoltage Start */

    /* End User Code Section: ADCDispatcher/ADC0_ChannelVoltage Start */
    Voltage_t return_value = ADC0_ChannelVoltage_array[index];
    /* Begin User Code Section: ADCDispatcher/ADC0_ChannelVoltage End */

    /* End User Code Section: ADCDispatcher/ADC0_ChannelVoltage End */
    return return_value;
}

uint16_t ADCDispatcher_Read_ADC0_RawChannelData(uint32_t index)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADCDispatcher/ADC0_RawChannelData Start */

    /* End User Code Section: ADCDispatcher/ADC0_RawChannelData Start */
    uint16_t return_value = ADC0_RawChannelData_array[index];
    /* Begin User Code Section: ADCDispatcher/ADC0_RawChannelData End */

    /* End User Code Section: ADCDispatcher/ADC0_RawChannelData End */
    return return_value;
}

Voltage_t ADCDispatcher_Read_ADC1_ChannelVoltage(uint32_t index)
{
    ASSERT(index < 8);
    /* Begin User Code Section: ADCDispatcher/ADC1_ChannelVoltage Start */

    /* End User Code Section: ADCDispatcher/ADC1_ChannelVoltage Start */
    Voltage_t return_value = ADC1_ChannelVoltage_array[index];
    /* Begin User Code Section: ADCDispatcher/ADC1_ChannelVoltage End */

    /* End User Code Section: ADCDispatcher/ADC1_ChannelVoltage End */
    return return_value;
}

uint16_t ADCDispatcher_Read_ADC1_RawChannelData(uint32_t index)
{
    ASSERT(index < 8);
    /* Begin User Code Section: ADCDispatcher/ADC1_RawChannelData Start */

    /* End User Code Section: ADCDispatcher/ADC1_RawChannelData Start */
    uint16_t return_value = ADC1_RawChannelData_array[index];
    /* Begin User Code Section: ADCDispatcher/ADC1_RawChannelData End */

    /* End User Code Section: ADCDispatcher/ADC1_RawChannelData End */
    return return_value;
}

void BatteryCalculator_Read_MainBatteryParameters(BatteryConfiguration_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: BatteryCalculator/MainBatteryParameters Start */

    /* End User Code Section: BatteryCalculator/MainBatteryParameters Start */
    ProjectConfiguration_Constant_MainBatteryParameters(value);
    /* Begin User Code Section: BatteryCalculator/MainBatteryParameters End */

    /* End User Code Section: BatteryCalculator/MainBatteryParameters End */
}

Voltage_t BatteryCalculator_Read_MainBatteryVoltage(void)
{
    /* Begin User Code Section: BatteryCalculator/MainBatteryVoltage Start */

    /* End User Code Section: BatteryCalculator/MainBatteryVoltage Start */
    Voltage_t return_value = ADCDispatcher_MainBatteryVoltage_variable;
    /* Begin User Code Section: BatteryCalculator/MainBatteryVoltage End */

    /* End User Code Section: BatteryCalculator/MainBatteryVoltage End */
    return return_value;
}

void BatteryCalculator_Read_MotorBatteryParameters(BatteryConfiguration_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: BatteryCalculator/MotorBatteryParameters Start */

    /* End User Code Section: BatteryCalculator/MotorBatteryParameters Start */
    ProjectConfiguration_Constant_MotorBatteryParameters(value);
    /* Begin User Code Section: BatteryCalculator/MotorBatteryParameters End */

    /* End User Code Section: BatteryCalculator/MotorBatteryParameters End */
}

Voltage_t BatteryCalculator_Read_MotorBatteryVoltage(void)
{
    /* Begin User Code Section: BatteryCalculator/MotorBatteryVoltage Start */

    /* End User Code Section: BatteryCalculator/MotorBatteryVoltage Start */
    Voltage_t return_value = ADCDispatcher_MotorBatteryVoltage_variable;
    /* Begin User Code Section: BatteryCalculator/MotorBatteryVoltage End */

    /* End User Code Section: BatteryCalculator/MotorBatteryVoltage End */
    return return_value;
}

void DriveRequestMultiplexer_Read_DrivetrainDriveRequest(uint32_t index, DriveRequest_t* value)
{
    ASSERT(index < 6);
    ASSERT(value != NULL);
    /* Begin User Code Section: DriveRequestMultiplexer/DrivetrainDriveRequest Start */

    /* End User Code Section: DriveRequestMultiplexer/DrivetrainDriveRequest Start */
    *value = DriveTrain_DriveRequest_array[index];
    /* Begin User Code Section: DriveRequestMultiplexer/DrivetrainDriveRequest End */

    /* End User Code Section: DriveRequestMultiplexer/DrivetrainDriveRequest End */
}

bool DriveRequestMultiplexer_Read_IsDrivetrainMotor(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: DriveRequestMultiplexer/IsDrivetrainMotor Start */

    /* End User Code Section: DriveRequestMultiplexer/IsDrivetrainMotor Start */
    bool return_value = DriveTrain_MotorUsed_array[index];
    /* Begin User Code Section: DriveRequestMultiplexer/IsDrivetrainMotor End */

    /* End User Code Section: DriveRequestMultiplexer/IsDrivetrainMotor End */
    return return_value;
}

void DriveRequestMultiplexer_Read_UserDriveRequest(uint32_t index, DriveRequest_t* value)
{
    ASSERT(index < 6);
    ASSERT(value != NULL);
    /* Begin User Code Section: DriveRequestMultiplexer/UserDriveRequest Start */

    /* End User Code Section: DriveRequestMultiplexer/UserDriveRequest Start */
    __disable_irq();
    *value = MotorPortHandler_DriveRequest_array[index];
    __enable_irq();
    /* Begin User Code Section: DriveRequestMultiplexer/UserDriveRequest End */

    /* End User Code Section: DriveRequestMultiplexer/UserDriveRequest End */
}

float DriveTrain_Read_YawAngle(void)
{
    /* Begin User Code Section: DriveTrain/YawAngle Start */

    /* End User Code Section: DriveTrain/YawAngle Start */
    float return_value = IMUOrientationEstimator_OrientationEulerDegrees_variable.yaw;
    /* Begin User Code Section: DriveTrain/YawAngle End */

    /* End User Code Section: DriveTrain/YawAngle End */
    return return_value;
}

QueueStatus_t GyroscopeOffsetCompensator_Read_AngularSpeeds(Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: GyroscopeOffsetCompensator/AngularSpeeds Start */

    /* End User Code Section: GyroscopeOffsetCompensator/AngularSpeeds Start */
    QueueStatus_t return_value = QueueStatus_Empty;
    bool was_overflow = IMU_GyroscopeSample_queue_overflow;
    if (IMU_GyroscopeSample_queue_data_valid)
    {
        IMU_GyroscopeSample_queue_overflow = false;
        *value = IMU_GyroscopeSample_queue;
        IMU_GyroscopeSample_queue_data_valid = false;
        if (was_overflow)
        {
            return_value = QueueStatus_Overflow;
        }
        else
        {
            return_value = QueueStatus_Ok;
        }
    }
    /* Begin User Code Section: GyroscopeOffsetCompensator/AngularSpeeds End */

    /* End User Code Section: GyroscopeOffsetCompensator/AngularSpeeds End */
    return return_value;
}

bool GyroscopeOffsetCompensator_Read_IsMoving(void)
{
    /* Begin User Code Section: GyroscopeOffsetCompensator/IsMoving Start */

    /* End User Code Section: GyroscopeOffsetCompensator/IsMoving Start */
    bool return_value = IMUMovementDetector_IsMoving_variable;
    /* Begin User Code Section: GyroscopeOffsetCompensator/IsMoving End */

    /* End User Code Section: GyroscopeOffsetCompensator/IsMoving End */
    return return_value;
}

QueueStatus_t IMUMovementDetector_Read_AngularSpeeds(Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: IMUMovementDetector/AngularSpeeds Start */

    /* End User Code Section: IMUMovementDetector/AngularSpeeds Start */
    QueueStatus_t return_value = QueueStatus_Empty;
    bool was_overflow = IMU_GyroscopeSample_queue1_overflow;
    if (IMU_GyroscopeSample_queue1_data_valid)
    {
        IMU_GyroscopeSample_queue1_overflow = false;
        *value = IMU_GyroscopeSample_queue1;
        IMU_GyroscopeSample_queue1_data_valid = false;
        if (was_overflow)
        {
            return_value = QueueStatus_Overflow;
        }
        else
        {
            return_value = QueueStatus_Ok;
        }
    }
    /* Begin User Code Section: IMUMovementDetector/AngularSpeeds End */

    /* End User Code Section: IMUMovementDetector/AngularSpeeds End */
    return return_value;
}

QueueStatus_t IMUOrientationEstimator_Read_Acceleration(Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: IMUOrientationEstimator/Acceleration Start */

    /* End User Code Section: IMUOrientationEstimator/Acceleration Start */
    QueueStatus_t return_value = QueueStatus_Empty;
    if (IMU_AccelerometerSample_queue_count > 0u)
    {
        size_t idx = (IMU_AccelerometerSample_queue_write_index - IMU_AccelerometerSample_queue_count) % 10u;
        --IMU_AccelerometerSample_queue_count;
        *value = IMU_AccelerometerSample_queue[idx];

        if (IMU_AccelerometerSample_queue_overflow)
        {
            IMU_AccelerometerSample_queue_overflow = false;
            return_value = QueueStatus_Overflow;
        }
        else
        {
            return_value = QueueStatus_Ok;
        }
    }
    /* Begin User Code Section: IMUOrientationEstimator/Acceleration End */

    /* End User Code Section: IMUOrientationEstimator/Acceleration End */
    return return_value;
}

QueueStatus_t IMUOrientationEstimator_Read_AngularSpeeds(Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: IMUOrientationEstimator/AngularSpeeds Start */

    /* End User Code Section: IMUOrientationEstimator/AngularSpeeds Start */
    QueueStatus_t return_value = QueueStatus_Empty;
    if (GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_count > 0u)
    {
        size_t idx = (GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_write_index - GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_count) % 10u;
        --GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_count;
        *value = GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue[idx];

        if (GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_overflow)
        {
            GyroscopeOffsetCompensator_CompensatedAngularSpeeds_queue_overflow = false;
            return_value = QueueStatus_Overflow;
        }
        else
        {
            return_value = QueueStatus_Ok;
        }
    }
    /* Begin User Code Section: IMUOrientationEstimator/AngularSpeeds End */

    /* End User Code Section: IMUOrientationEstimator/AngularSpeeds End */
    return return_value;
}

float IMUOrientationEstimator_Read_SampleTime(void)
{
    /* Begin User Code Section: IMUOrientationEstimator/SampleTime Start */

    /* End User Code Section: IMUOrientationEstimator/SampleTime Start */
    /* Begin User Code Section: IMUOrientationEstimator/SampleTime End */

    /* End User Code Section: IMUOrientationEstimator/SampleTime End */
    return IMU_Constant_SampleTime();
}

rgb_t LEDController_Read_Colors(uint32_t index)
{
    ASSERT(index < 16);
    /* Begin User Code Section: LEDController/Colors Start */

    /* End User Code Section: LEDController/Colors Start */
    rgb_t return_value = LedDisplayController_Leds_array[index];
    /* Begin User Code Section: LEDController/Colors End */

    /* End User Code Section: LEDController/Colors End */
    return return_value;
}

uint8_t LEDController_Read_MaxBrightness(void)
{
    /* Begin User Code Section: LEDController/MaxBrightness Start */

    /* End User Code Section: LEDController/MaxBrightness Start */
    uint8_t return_value = LedDisplayController_MaxBrightness_variable;
    /* Begin User Code Section: LEDController/MaxBrightness End */

    /* End User Code Section: LEDController/MaxBrightness End */
    return return_value;
}

BluetoothStatus_t LedDisplayController_Read_BluetoothStatus(void)
{
    /* Begin User Code Section: LedDisplayController/BluetoothStatus Start */

    /* End User Code Section: LedDisplayController/BluetoothStatus Start */
    BluetoothStatus_t return_value = BluetoothStatusObserver_ConnectionStatus_variable;
    /* Begin User Code Section: LedDisplayController/BluetoothStatus End */

    /* End User Code Section: LedDisplayController/BluetoothStatus End */
    return return_value;
}

uint8_t LedDisplayController_Read_MainBatteryLevel(void)
{
    /* Begin User Code Section: LedDisplayController/MainBatteryLevel Start */

    /* End User Code Section: LedDisplayController/MainBatteryLevel Start */
    uint8_t return_value = BatteryCalculator_MainBatteryLevel_variable;
    /* Begin User Code Section: LedDisplayController/MainBatteryLevel End */

    /* End User Code Section: LedDisplayController/MainBatteryLevel End */
    return return_value;
}

bool LedDisplayController_Read_MainBatteryLow(void)
{
    /* Begin User Code Section: LedDisplayController/MainBatteryLow Start */

    /* End User Code Section: LedDisplayController/MainBatteryLow Start */
    bool return_value = BatteryCalculator_MainBatteryLow_variable;
    /* Begin User Code Section: LedDisplayController/MainBatteryLow End */

    /* End User Code Section: LedDisplayController/MainBatteryLow End */
    return return_value;
}

ChargerState_t LedDisplayController_Read_MainBatteryStatus(void)
{
    /* Begin User Code Section: LedDisplayController/MainBatteryStatus Start */

    /* End User Code Section: LedDisplayController/MainBatteryStatus Start */
    ChargerState_t return_value = BatteryCharger_ChargerState_variable;
    /* Begin User Code Section: LedDisplayController/MainBatteryStatus End */

    /* End User Code Section: LedDisplayController/MainBatteryStatus End */
    return return_value;
}

MasterStatus_t LedDisplayController_Read_MasterStatus(void)
{
    /* Begin User Code Section: LedDisplayController/MasterStatus Start */

    /* End User Code Section: LedDisplayController/MasterStatus Start */
    MasterStatus_t return_value = MasterStatusObserver_MasterStatus_variable;
    /* Begin User Code Section: LedDisplayController/MasterStatus End */

    /* End User Code Section: LedDisplayController/MasterStatus End */
    return return_value;
}

uint8_t LedDisplayController_Read_MotorBatteryLevel(void)
{
    /* Begin User Code Section: LedDisplayController/MotorBatteryLevel Start */

    /* End User Code Section: LedDisplayController/MotorBatteryLevel Start */
    uint8_t return_value = BatteryCalculator_MotorBatteryLevel_variable;
    /* Begin User Code Section: LedDisplayController/MotorBatteryLevel End */

    /* End User Code Section: LedDisplayController/MotorBatteryLevel End */
    return return_value;
}

bool LedDisplayController_Read_MotorBatteryPresent(void)
{
    /* Begin User Code Section: LedDisplayController/MotorBatteryPresent Start */

    /* End User Code Section: LedDisplayController/MotorBatteryPresent Start */
    bool return_value = BatteryCalculator_MotorBatteryPresent_variable;
    /* Begin User Code Section: LedDisplayController/MotorBatteryPresent End */

    /* End User Code Section: LedDisplayController/MotorBatteryPresent End */
    return return_value;
}

int8_t LedDisplayController_Read_MotorDriveValues(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: LedDisplayController/MotorDriveValues Start */

    /* End User Code Section: LedDisplayController/MotorDriveValues Start */
    int8_t return_value = MotorPortHandler_DriveStrength_array[index];
    /* Begin User Code Section: LedDisplayController/MotorDriveValues End */

    /* End User Code Section: LedDisplayController/MotorDriveValues End */
    return return_value;
}

rgb_t LedDisplayController_Read_RingLeds(uint32_t index)
{
    ASSERT(index < 12);
    /* Begin User Code Section: LedDisplayController/RingLeds Start */

    /* End User Code Section: LedDisplayController/RingLeds Start */
    rgb_t return_value = RingLedDisplay_LedColor_array[index];
    /* Begin User Code Section: LedDisplayController/RingLeds End */

    /* End User Code Section: LedDisplayController/RingLeds End */
    return return_value;
}

SlotData_t McuStatusCollector_Read_SlotData(uint32_t index)
{
    ASSERT(index < 14);
    /* Begin User Code Section: McuStatusCollector/SlotData Start */

    /* End User Code Section: McuStatusCollector/SlotData Start */
    SlotData_t return_value = McuStatusSlots_SlotData_array[index];
    /* Begin User Code Section: McuStatusCollector/SlotData End */

    /* End User Code Section: McuStatusCollector/SlotData End */
    return return_value;
}

void McuStatusSlots_Read_Acceleration(IMU_RawSample_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: McuStatusSlots/Acceleration Start */

    /* End User Code Section: McuStatusSlots/Acceleration Start */
    *value = IMU_RawAccelerometerSample_variable;
    /* Begin User Code Section: McuStatusSlots/Acceleration End */

    /* End User Code Section: McuStatusSlots/Acceleration End */
}

void McuStatusSlots_Read_AngularSpeeds(IMU_RawSample_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: McuStatusSlots/AngularSpeeds Start */

    /* End User Code Section: McuStatusSlots/AngularSpeeds Start */
    *value = IMU_RawGyroscopeSample_variable;
    /* Begin User Code Section: McuStatusSlots/AngularSpeeds End */

    /* End User Code Section: McuStatusSlots/AngularSpeeds End */
}

uint8_t McuStatusSlots_Read_MainBatteryLevel(void)
{
    /* Begin User Code Section: McuStatusSlots/MainBatteryLevel Start */

    /* End User Code Section: McuStatusSlots/MainBatteryLevel Start */
    uint8_t return_value = BatteryCalculator_MainBatteryLevel_variable;
    /* Begin User Code Section: McuStatusSlots/MainBatteryLevel End */

    /* End User Code Section: McuStatusSlots/MainBatteryLevel End */
    return return_value;
}

ChargerState_t McuStatusSlots_Read_MainBatteryStatus(void)
{
    /* Begin User Code Section: McuStatusSlots/MainBatteryStatus Start */

    /* End User Code Section: McuStatusSlots/MainBatteryStatus Start */
    ChargerState_t return_value = BatteryCharger_ChargerState_variable;
    /* Begin User Code Section: McuStatusSlots/MainBatteryStatus End */

    /* End User Code Section: McuStatusSlots/MainBatteryStatus End */
    return return_value;
}

uint8_t McuStatusSlots_Read_MotorBatteryLevel(void)
{
    /* Begin User Code Section: McuStatusSlots/MotorBatteryLevel Start */

    /* End User Code Section: McuStatusSlots/MotorBatteryLevel Start */
    uint8_t return_value = BatteryCalculator_MotorBatteryLevel_variable;
    /* Begin User Code Section: McuStatusSlots/MotorBatteryLevel End */

    /* End User Code Section: McuStatusSlots/MotorBatteryLevel End */
    return return_value;
}

float McuStatusSlots_Read_YawAngle(void)
{
    /* Begin User Code Section: McuStatusSlots/YawAngle Start */

    /* End User Code Section: McuStatusSlots/YawAngle Start */
    float return_value = IMUOrientationEstimator_OrientationEulerDegrees_variable.yaw;
    /* Begin User Code Section: McuStatusSlots/YawAngle End */

    /* End User Code Section: McuStatusSlots/YawAngle End */
    return return_value;
}

Current_t MotorCurrentFilter_Read_RawCurrent(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorCurrentFilter/RawCurrent Start */

    /* End User Code Section: MotorCurrentFilter/RawCurrent Start */
    Current_t return_value = ADCDispatcher_MotorCurrent_array[index];
    /* Begin User Code Section: MotorCurrentFilter/RawCurrent End */

    /* End User Code Section: MotorCurrentFilter/RawCurrent End */
    return return_value;
}

int8_t MotorDerating_Read_ControlValue(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorDerating/ControlValue Start */

    /* End User Code Section: MotorDerating/ControlValue Start */
    int8_t return_value = MotorPortHandler_DriveStrength_array[index];
    /* Begin User Code Section: MotorDerating/ControlValue End */

    /* End User Code Section: MotorDerating/ControlValue End */
    return return_value;
}

Temperature_t MotorDerating_Read_MotorTemperature(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorDerating/MotorTemperature Start */

    /* End User Code Section: MotorDerating/MotorTemperature Start */
    Temperature_t return_value = MotorThermalModel_Temperature_array[index];
    /* Begin User Code Section: MotorDerating/MotorTemperature End */

    /* End User Code Section: MotorDerating/MotorTemperature End */
    return return_value;
}

void MotorDerating_Read_Parameters(MotorDeratingParameters_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: MotorDerating/Parameters Start */

    /* End User Code Section: MotorDerating/Parameters Start */
    ProjectConfiguration_Constant_MotorDeratingParameters(value);
    /* Begin User Code Section: MotorDerating/Parameters End */

    /* End User Code Section: MotorDerating/Parameters End */
}

void MotorPortHandler_Read_AppliedDriveRequest(uint32_t index, DriveRequest_t* value)
{
    ASSERT(index < 6);
    ASSERT(value != NULL);
    /* Begin User Code Section: MotorPortHandler/AppliedDriveRequest Start */

    /* End User Code Section: MotorPortHandler/AppliedDriveRequest Start */
    *value = DriveRequestMultiplexer_AppliedDriveRequest_array[index];
    /* Begin User Code Section: MotorPortHandler/AppliedDriveRequest End */

    /* End User Code Section: MotorPortHandler/AppliedDriveRequest End */
}

Current_t MotorThermalModel_Read_MotorCurrent(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorThermalModel/MotorCurrent Start */

    /* End User Code Section: MotorThermalModel/MotorCurrent Start */
    Current_t return_value = MotorCurrentFilter_FilteredCurrent_array[index];
    /* Begin User Code Section: MotorThermalModel/MotorCurrent End */

    /* End User Code Section: MotorThermalModel/MotorCurrent End */
    return return_value;
}

void MotorThermalModel_Read_ThermalParameters(MotorThermalParameters_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: MotorThermalModel/ThermalParameters Start */

    /* End User Code Section: MotorThermalModel/ThermalParameters Start */
    ProjectConfiguration_Constant_MotorThermalParameters(value);
    /* Begin User Code Section: MotorThermalModel/ThermalParameters End */

    /* End User Code Section: MotorThermalModel/ThermalParameters End */
}

uint32_t RingLedDisplay_Read_ExpectedStartupTime(void)
{
    /* Begin User Code Section: RingLedDisplay/ExpectedStartupTime Start */

    /* End User Code Section: RingLedDisplay/ExpectedStartupTime Start */
    /* Begin User Code Section: RingLedDisplay/ExpectedStartupTime End */

    /* End User Code Section: RingLedDisplay/ExpectedStartupTime End */
    return ProjectConfiguration_Constant_ExpectedStartupTime();
}

bool RingLedDisplay_Read_MasterReady(void)
{
    /* Begin User Code Section: RingLedDisplay/MasterReady Start */

    /* End User Code Section: RingLedDisplay/MasterReady Start */
    bool return_value = CommunicationObserver_Enabled_variable;
    /* Begin User Code Section: RingLedDisplay/MasterReady End */

    /* End User Code Section: RingLedDisplay/MasterReady End */
    return return_value;
}

RingLedScenario_t RingLedDisplay_Read_Scenario(void)
{
    /* Begin User Code Section: RingLedDisplay/Scenario Start */

    /* End User Code Section: RingLedDisplay/Scenario Start */
    __disable_irq();
    RingLedScenario_t return_value = CommWrapper_LedDisplay_Scenario_variable;
    __enable_irq();
    /* Begin User Code Section: RingLedDisplay/Scenario End */

    /* End User Code Section: RingLedDisplay/Scenario End */
    return return_value;
}

rgb_t RingLedDisplay_Read_UserColors(uint32_t index)
{
    ASSERT(index < 12);
    /* Begin User Code Section: RingLedDisplay/UserColors Start */

    /* End User Code Section: RingLedDisplay/UserColors Start */
    __disable_irq();
    rgb_t return_value = CommWrapper_LedDisplay_UserFrame_array[index];
    __enable_irq();
    /* Begin User Code Section: RingLedDisplay/UserColors End */

    /* End User Code Section: RingLedDisplay/UserColors End */
    return return_value;
}

bool RingLedDisplay_Read_WaitForMasterStartup(void)
{
    /* Begin User Code Section: RingLedDisplay/WaitForMasterStartup Start */

    /* End User Code Section: RingLedDisplay/WaitForMasterStartup Start */
    bool return_value = StartupReasonProvider_IsColdStart_variable;
    /* Begin User Code Section: RingLedDisplay/WaitForMasterStartup End */

    /* End User Code Section: RingLedDisplay/WaitForMasterStartup End */
    return return_value;
}

uint8_t SensorPortHandler_Read_AdcData(uint32_t index)
{
    ASSERT(index < 4);
    /* Begin User Code Section: SensorPortHandler/AdcData Start */

    /* End User Code Section: SensorPortHandler/AdcData Start */
    uint8_t return_value = ADCDispatcher_Sensor_ADC_array[index];
    /* Begin User Code Section: SensorPortHandler/AdcData End */

    /* End User Code Section: SensorPortHandler/AdcData End */
    return return_value;
}
