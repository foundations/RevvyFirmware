#include "generated_runtime.h"
#include "utils.h"

/* Begin User Code Section: Declarations */

/* End User Code Section: Declarations */
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
static Current_t MotorCurrentFilter_FilteredCurrent_MotorThermalModel_MotorCurrent_array[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static Temperature_t MotorThermalModel_Temperature_MotorDerating_MotorTemperature_array[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

void Runtime_Call_OnInit(void)
{
    /* Begin User Code Section: Runtime/OnInit Start */

    /* End User Code Section: Runtime/OnInit Start */
    ErrorStorage_Run_OnInit();
    ADC0_Run_OnInit();
    ADC1_Run_OnInit();
    MotorCurrentFilter_Run_OnInit();
    BatteryCharger_Run_OnInit();
    LEDController_Run_OnInit();
    BatteryCalculator_Run_OnInit();
    BluetoothIndicator_Run_OnInit();
    BrainStatusIndicator_Run_Update();
    IMU_Run_OnInit();
    HighResolutionTimer_Run_OnInit();
    MasterStatusObserver_Run_OnInit();
    MotorThermalModel_Run_OnInit();
    BluetoothStatusObserver_Run_OnInit();
    RingLedDisplay_Run_OnInit();
    CommunicationObserver_Run_OnInit();
    DriveTrain_Run_OnInit();
    GyroscopeOffsetCompensator_Run_OnInit();
    YawAngleTracker_Run_OnInit();
    MasterCommunicationInterface_Run_OnInit();
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
    GyroscopeOffsetCompensator_Run_Update();
    YawAngleTracker_Run_Update();
    ADCDispatcher_Run_Update();
    MotorCurrentFilter_Run_Update();
    MotorThermalModel_Run_OnUpdate();
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
    /* Begin User Code Section: Runtime/10ms_offset2 End */

    /* End User Code Section: Runtime/10ms_offset2 End */
}

void Runtime_Call_10ms_offset3(void)
{
    /* Begin User Code Section: Runtime/10ms_offset3 Start */

    /* End User Code Section: Runtime/10ms_offset3 Start */
    /* Begin User Code Section: Runtime/10ms_offset3 End */

    /* End User Code Section: Runtime/10ms_offset3 End */
}

void Runtime_Call_10ms_offset4(void)
{
    /* Begin User Code Section: Runtime/10ms_offset4 Start */

    /* End User Code Section: Runtime/10ms_offset4 Start */
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
    /* Begin User Code Section: Runtime/20ms_offset5 End */

    /* End User Code Section: Runtime/20ms_offset5 End */
}

void Runtime_Call_20ms_offset6(void)
{
    /* Begin User Code Section: Runtime/20ms_offset6 Start */

    /* End User Code Section: Runtime/20ms_offset6 Start */
    /* Begin User Code Section: Runtime/20ms_offset6 End */

    /* End User Code Section: Runtime/20ms_offset6 End */
}

void Runtime_Call_20ms_offset7(void)
{
    /* Begin User Code Section: Runtime/20ms_offset7 Start */

    /* End User Code Section: Runtime/20ms_offset7 Start */
    /* Begin User Code Section: Runtime/20ms_offset7 End */

    /* End User Code Section: Runtime/20ms_offset7 End */
}

void Runtime_Call_20ms_offset8(void)
{
    /* Begin User Code Section: Runtime/20ms_offset8 Start */

    /* End User Code Section: Runtime/20ms_offset8 Start */
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
    BluetoothIndicator_Run_Update();
    BrainStatusIndicator_Run_Update();
    /* Begin User Code Section: Runtime/100ms End */

    /* End User Code Section: Runtime/100ms End */
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

void ADC0_Write_ChannelVoltage(uint32_t index, const Voltage_t value)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADC0/ChannelVoltage Start */

    /* End User Code Section: ADC0/ChannelVoltage Start */
    ADC0_ChannelVoltage_ADCDispatcher_ADC0_ChannelVoltage_array[index] = value;
    /* Begin User Code Section: ADC0/ChannelVoltage End */

    /* End User Code Section: ADC0/ChannelVoltage End */
}

void ADC0_Write_RawChannelData(uint32_t index, const uint16_t value)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADC0/RawChannelData Start */

    /* End User Code Section: ADC0/RawChannelData Start */
    ADC0_RawChannelData_ADCDispatcher_ADC0_RawChannelData_array[index] = value;
    /* Begin User Code Section: ADC0/RawChannelData End */

    /* End User Code Section: ADC0/RawChannelData End */
}

void ADC1_Write_ChannelVoltage(uint32_t index, const Voltage_t value)
{
    ASSERT(index < 8);
    /* Begin User Code Section: ADC1/ChannelVoltage Start */

    /* End User Code Section: ADC1/ChannelVoltage Start */
    ADC1_ChannelVoltage_ADCDispatcher_ADC1_ChannelVoltage_array[index] = value;
    /* Begin User Code Section: ADC1/ChannelVoltage End */

    /* End User Code Section: ADC1/ChannelVoltage End */
}

void ADC1_Write_RawChannelData(uint32_t index, const uint16_t value)
{
    ASSERT(index < 8);
    /* Begin User Code Section: ADC1/RawChannelData Start */

    /* End User Code Section: ADC1/RawChannelData Start */
    ADC1_RawChannelData_ADCDispatcher_ADC1_RawChannelData_array[index] = value;
    /* Begin User Code Section: ADC1/RawChannelData End */

    /* End User Code Section: ADC1/RawChannelData End */
}

void ADCDispatcher_Write_MainBatteryVoltage(const Voltage_t value)
{
    /* Begin User Code Section: ADCDispatcher/MainBatteryVoltage Start */

    /* End User Code Section: ADCDispatcher/MainBatteryVoltage Start */
    ADCDispatcher_MainBatteryVoltage_BatteryCalculator_MainBatteryVoltage_variable = value;
    /* Begin User Code Section: ADCDispatcher/MainBatteryVoltage End */

    /* End User Code Section: ADCDispatcher/MainBatteryVoltage End */
}

void ADCDispatcher_Write_MotorBatteryVoltage(const Voltage_t value)
{
    /* Begin User Code Section: ADCDispatcher/MotorBatteryVoltage Start */

    /* End User Code Section: ADCDispatcher/MotorBatteryVoltage Start */
    ADCDispatcher_MotorBatteryVoltage_BatteryCalculator_MotorBatteryVoltage_variable = value;
    /* Begin User Code Section: ADCDispatcher/MotorBatteryVoltage End */

    /* End User Code Section: ADCDispatcher/MotorBatteryVoltage End */
}

void ADCDispatcher_Write_MotorCurrent(uint32_t index, const Current_t value)
{
    ASSERT(index < 6);
    /* Begin User Code Section: ADCDispatcher/MotorCurrent Start */

    /* End User Code Section: ADCDispatcher/MotorCurrent Start */
    ADCDispatcher_MotorCurrent_MotorCurrentFilter_RawCurrent_array[index] = value;
    /* Begin User Code Section: ADCDispatcher/MotorCurrent End */

    /* End User Code Section: ADCDispatcher/MotorCurrent End */
}

void ADCDispatcher_Write_Sensor_ADC(uint32_t index, const uint8_t value)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADCDispatcher/Sensor_ADC Start */

    /* End User Code Section: ADCDispatcher/Sensor_ADC Start */
    ADCDispatcher_Sensor_ADC_SensorPortHandler_AdcData_array[index] = value;
    /* Begin User Code Section: ADCDispatcher/Sensor_ADC End */

    /* End User Code Section: ADCDispatcher/Sensor_ADC End */
}

void BatteryCharger_Write_ChargerState(const ChargerState_t value)
{
    /* Begin User Code Section: BatteryCharger/ChargerState Start */

    /* End User Code Section: BatteryCharger/ChargerState Start */
    BatteryCharger_ChargerState_BatteryStatusProvider_IsMainBatteryCharging_variable = value;
    /* Begin User Code Section: BatteryCharger/ChargerState End */

    /* End User Code Section: BatteryCharger/ChargerState End */
}

void BluetoothStatusObserver_Write_ConnectionStatus(const BluetoothStatus_t value)
{
    /* Begin User Code Section: BluetoothStatusObserver/ConnectionStatus Start */

    /* End User Code Section: BluetoothStatusObserver/ConnectionStatus Start */
    BluetoothStatusObserver_ConnectionStatus_BluetoothIndicator_ConnectionStatus_variable = value;
    /* Begin User Code Section: BluetoothStatusObserver/ConnectionStatus End */

    /* End User Code Section: BluetoothStatusObserver/ConnectionStatus End */
}

void CommunicationObserver_Write_Enabled(const bool value)
{
    /* Begin User Code Section: CommunicationObserver/Enabled Start */

    /* End User Code Section: CommunicationObserver/Enabled Start */
    CommunicationObserver_Enabled_RingLedDisplay_MasterReady_variable = value;
    /* Begin User Code Section: CommunicationObserver/Enabled End */

    /* End User Code Section: CommunicationObserver/Enabled End */
}

void IMU_Write_GyroscopeSample(const Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: IMU/GyroscopeSample Start */

    /* End User Code Section: IMU/GyroscopeSample Start */
    IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_overflow = IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_data_valid;
    IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue = *value;
    IMU_GyroscopeSample_GyroscopeOffsetCompensator_AngularSpeeds_queue_data_valid = true;
    /* Begin User Code Section: IMU/GyroscopeSample End */

    /* End User Code Section: IMU/GyroscopeSample End */
}

void MotorCurrentFilter_Write_FilteredCurrent(uint32_t index, const Current_t value)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorCurrentFilter/FilteredCurrent Start */

    /* End User Code Section: MotorCurrentFilter/FilteredCurrent Start */
    MotorCurrentFilter_FilteredCurrent_MotorThermalModel_MotorCurrent_array[index] = value;
    /* Begin User Code Section: MotorCurrentFilter/FilteredCurrent End */

    /* End User Code Section: MotorCurrentFilter/FilteredCurrent End */
}

void MotorDerating_Write_DeratedControlValue(uint32_t index, const int8_t value)
{
    (void) value;
    (void) index;
    ASSERT(index < 6);
    /* Begin User Code Section: MotorDerating/DeratedControlValue Start */

    /* End User Code Section: MotorDerating/DeratedControlValue Start */
    /* Begin User Code Section: MotorDerating/DeratedControlValue End */

    /* End User Code Section: MotorDerating/DeratedControlValue End */
}

void MotorThermalModel_Write_Temperature(uint32_t index, const Temperature_t value)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorThermalModel/Temperature Start */

    /* End User Code Section: MotorThermalModel/Temperature Start */
    MotorThermalModel_Temperature_MotorDerating_MotorTemperature_array[index] = value;
    /* Begin User Code Section: MotorThermalModel/Temperature End */

    /* End User Code Section: MotorThermalModel/Temperature End */
}

Voltage_t ADCDispatcher_Read_ADC0_ChannelVoltage(uint32_t index)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADCDispatcher/ADC0_ChannelVoltage Start */

    /* End User Code Section: ADCDispatcher/ADC0_ChannelVoltage Start */
    Voltage_t return_value = ADC0_ChannelVoltage_ADCDispatcher_ADC0_ChannelVoltage_array[index];
    /* Begin User Code Section: ADCDispatcher/ADC0_ChannelVoltage End */

    /* End User Code Section: ADCDispatcher/ADC0_ChannelVoltage End */
    return return_value;
}

uint16_t ADCDispatcher_Read_ADC0_RawChannelData(uint32_t index)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADCDispatcher/ADC0_RawChannelData Start */

    /* End User Code Section: ADCDispatcher/ADC0_RawChannelData Start */
    uint16_t return_value = ADC0_RawChannelData_ADCDispatcher_ADC0_RawChannelData_array[index];
    /* Begin User Code Section: ADCDispatcher/ADC0_RawChannelData End */

    /* End User Code Section: ADCDispatcher/ADC0_RawChannelData End */
    return return_value;
}

Voltage_t ADCDispatcher_Read_ADC1_ChannelVoltage(uint32_t index)
{
    ASSERT(index < 8);
    /* Begin User Code Section: ADCDispatcher/ADC1_ChannelVoltage Start */

    /* End User Code Section: ADCDispatcher/ADC1_ChannelVoltage Start */
    Voltage_t return_value = ADC1_ChannelVoltage_ADCDispatcher_ADC1_ChannelVoltage_array[index];
    /* Begin User Code Section: ADCDispatcher/ADC1_ChannelVoltage End */

    /* End User Code Section: ADCDispatcher/ADC1_ChannelVoltage End */
    return return_value;
}

uint16_t ADCDispatcher_Read_ADC1_RawChannelData(uint32_t index)
{
    ASSERT(index < 8);
    /* Begin User Code Section: ADCDispatcher/ADC1_RawChannelData Start */

    /* End User Code Section: ADCDispatcher/ADC1_RawChannelData Start */
    uint16_t return_value = ADC1_RawChannelData_ADCDispatcher_ADC1_RawChannelData_array[index];
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
    Voltage_t return_value = ADCDispatcher_MainBatteryVoltage_BatteryCalculator_MainBatteryVoltage_variable;
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
    Voltage_t return_value = ADCDispatcher_MotorBatteryVoltage_BatteryCalculator_MotorBatteryVoltage_variable;
    /* Begin User Code Section: BatteryCalculator/MotorBatteryVoltage End */

    /* End User Code Section: BatteryCalculator/MotorBatteryVoltage End */
    return return_value;
}

ChargerState_t BatteryStatusProvider_Read_IsMainBatteryCharging(void)
{
    /* Begin User Code Section: BatteryStatusProvider/IsMainBatteryCharging Start */

    /* End User Code Section: BatteryStatusProvider/IsMainBatteryCharging Start */
    ChargerState_t return_value = BatteryCharger_ChargerState_BatteryStatusProvider_IsMainBatteryCharging_variable;
    /* Begin User Code Section: BatteryStatusProvider/IsMainBatteryCharging End */

    /* End User Code Section: BatteryStatusProvider/IsMainBatteryCharging End */
    return return_value;
}

BluetoothStatus_t BluetoothIndicator_Read_ConnectionStatus(void)
{
    /* Begin User Code Section: BluetoothIndicator/ConnectionStatus Start */

    /* End User Code Section: BluetoothIndicator/ConnectionStatus Start */
    BluetoothStatus_t return_value = BluetoothStatusObserver_ConnectionStatus_BluetoothIndicator_ConnectionStatus_variable;
    /* Begin User Code Section: BluetoothIndicator/ConnectionStatus End */

    /* End User Code Section: BluetoothIndicator/ConnectionStatus End */
    return return_value;
}

QueueStatus_t GyroscopeOffsetCompensator_Read_AngularSpeeds(Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: GyroscopeOffsetCompensator/AngularSpeeds Start */

    /* End User Code Section: GyroscopeOffsetCompensator/AngularSpeeds Start */
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
    /* Begin User Code Section: GyroscopeOffsetCompensator/AngularSpeeds End */

    /* End User Code Section: GyroscopeOffsetCompensator/AngularSpeeds End */
    return return_value;
}

Current_t MotorCurrentFilter_Read_RawCurrent(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorCurrentFilter/RawCurrent Start */

    /* End User Code Section: MotorCurrentFilter/RawCurrent Start */
    Current_t return_value = ADCDispatcher_MotorCurrent_MotorCurrentFilter_RawCurrent_array[index];
    /* Begin User Code Section: MotorCurrentFilter/RawCurrent End */

    /* End User Code Section: MotorCurrentFilter/RawCurrent End */
    return return_value;
}

Temperature_t MotorDerating_Read_MotorTemperature(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorDerating/MotorTemperature Start */

    /* End User Code Section: MotorDerating/MotorTemperature Start */
    Temperature_t return_value = MotorThermalModel_Temperature_MotorDerating_MotorTemperature_array[index];
    /* Begin User Code Section: MotorDerating/MotorTemperature End */

    /* End User Code Section: MotorDerating/MotorTemperature End */
    return return_value;
}

Current_t MotorThermalModel_Read_MotorCurrent(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorThermalModel/MotorCurrent Start */

    /* End User Code Section: MotorThermalModel/MotorCurrent Start */
    Current_t return_value = MotorCurrentFilter_FilteredCurrent_MotorThermalModel_MotorCurrent_array[index];
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

bool RingLedDisplay_Read_MasterReady(void)
{
    /* Begin User Code Section: RingLedDisplay/MasterReady Start */

    /* End User Code Section: RingLedDisplay/MasterReady Start */
    bool return_value = CommunicationObserver_Enabled_RingLedDisplay_MasterReady_variable;
    /* Begin User Code Section: RingLedDisplay/MasterReady End */

    /* End User Code Section: RingLedDisplay/MasterReady End */
    return return_value;
}

uint8_t SensorPortHandler_Read_AdcData(uint32_t index)
{
    ASSERT(index < 4);
    /* Begin User Code Section: SensorPortHandler/AdcData Start */

    /* End User Code Section: SensorPortHandler/AdcData Start */
    uint8_t return_value = ADCDispatcher_Sensor_ADC_SensorPortHandler_AdcData_array[index];
    /* Begin User Code Section: SensorPortHandler/AdcData End */

    /* End User Code Section: SensorPortHandler/AdcData End */
    return return_value;
}

float YawAngleTracker_Read_SampleTime(void)
{
    /* Begin User Code Section: YawAngleTracker/SampleTime Start */

    /* End User Code Section: YawAngleTracker/SampleTime Start */
    /* Begin User Code Section: YawAngleTracker/SampleTime End */

    /* End User Code Section: YawAngleTracker/SampleTime End */
    return IMU_Constant_SampleTime();
}
