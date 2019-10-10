#ifndef GENERATED_RUNTIME_H_
#define GENERATED_RUNTIME_H_

#include "utils/color.h"
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef float Voltage_t;
typedef float Current_t;

typedef struct {
    Voltage_t detectionVoltage;
    Voltage_t minVoltage;
    Voltage_t maxVoltage;
} BatteryConfiguration_t;

typedef enum {
    ChargerState_NotPluggedIn,
    ChargerState_Charging,
    ChargerState_Charged,
    ChargerState_Fault
} ChargerState_t;

typedef enum {
    BluetoothStatus_Inactive,
    BluetoothStatus_NotConnected,
    BluetoothStatus_Connected
} BluetoothStatus_t;

typedef struct {
    float x;
    float y;
    float z;
} Vector3D_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} IMU_RawSample_t;

typedef struct {
    float pitch;
    float roll;
    float yaw;
} Orientation3D_t;

typedef struct {
    float q0;
    float q1;
    float q2;
    float q3;
} Quaternion_t;

typedef enum {
    MasterStatus_Unknown,
    MasterStatus_Operational,
    MasterStatus_Controlled
} MasterStatus_t;

typedef struct {
    uint8_t* payload;
    size_t size;
} MasterMessage_t;

typedef struct {
    MasterMessage_t default_response;
    MasterMessage_t rx_overflow_response;
    uint32_t rx_timeout;
} MasterCommunicationInterface_Config_t;
typedef float Temperature_t;

typedef struct {
    Temperature_t MaxSafeTemperature;
    Temperature_t MaxAllowedTemperature;
} MotorDeratingParameters_t;

typedef struct {
    float resistance;
    float coeff_cooling;
    float coeff_heating;
} MotorThermalParameters_t;

typedef enum {
    RingLedScenario_Off,
    RingLedScenario_UserFrame,
    RingLedScenario_ColorWheel,
    RingLedScenario_RainbowFade,
    RingLedScenario_BusyIndicator,
    RingLedScenario_BreathingGreen
} RingLedScenario_t;

typedef enum {
    QueueStatus_Empty,
    QueueStatus_Ok,
    QueueStatus_Overflow
} QueueStatus_t;

#define COMPONENT_TYPES_ADC0_H_
#define COMPONENT_TYPES_ADC1_H_
#define COMPONENT_TYPES_ADC_DISPATCHER_H_
#define COMPONENT_TYPES_BATTERY_CALCULATOR_H_
#define COMPONENT_TYPES_BATTERY_CHARGER_H_
#define COMPONENT_TYPES_BATTERY_STATUS_PROVIDER_H_
#define COMPONENT_TYPES_BLUETOOTH_STATUS_OBSERVER_H_
#define COMPONENT_TYPES_COMMUNICATION_OBSERVER_H_
#define COMPONENT_TYPES_DRIVE_TRAIN_H_
#define COMPONENT_TYPES_ERROR_STORAGE_H_
#define COMPONENT_TYPES_ERROR_STORAGE_WRAPPER_H_
#define COMPONENT_TYPES_GYROSCOPE_OFFSET_COMPENSATOR_H_
#define COMPONENT_TYPES_HIGH_RESOLUTION_TIMER_H_
#define COMPONENT_TYPES_IMU_H_
#define COMPONENT_TYPES_IMU_MOVEMENT_DETECTOR_H_
#define COMPONENT_TYPES_IMU_ORIENTATION_ESTIMATOR_H_
#define COMPONENT_TYPES_INTERNAL_TEMPERATURE_SENSOR_H_
#define COMPONENT_TYPES_LED_CONTROLLER_H_
#define COMPONENT_TYPES_LED_DISPLAY_CONTROLLER_H_
#define COMPONENT_TYPES_MASTER_COMMUNICATION_H_
#define COMPONENT_TYPES_MASTER_COMMUNICATION_INTERFACE_H_
#define COMPONENT_TYPES_MASTER_STATUS_OBSERVER_H_
#define COMPONENT_TYPES_MCU_STATUS_COLLECTOR_H_
#define COMPONENT_TYPES_MCU_STATUS_COLLECTOR_WRAPPER_H_
#define COMPONENT_TYPES_MEMORY_ALLOCATOR_H_
#define COMPONENT_TYPES_MOTOR_CURRENT_FILTER_H_
#define COMPONENT_TYPES_MOTOR_DERATING_H_
#define COMPONENT_TYPES_MOTOR_DRIVER_8833_H_
#define COMPONENT_TYPES_MOTOR_PORT_HANDLER_H_
#define COMPONENT_TYPES_MOTOR_THERMAL_MODEL_H_
#define COMPONENT_TYPES_PROJECT_CONFIGURATION_H_
#define COMPONENT_TYPES_RESTART_MANAGER_H_
#define COMPONENT_TYPES_RING_LED_DISPLAY_H_
#define COMPONENT_TYPES_SENSOR_PORT_HANDLER_H_
#define COMPONENT_TYPES_VERSION_PROVIDER_H_
#define COMPONENT_TYPES_WATCHDOG_FEEDER_H_
#define COMPONENT_TYPES_YAW_ANGLE_TRACKER_H_

#include "components/ADC0/ADC0.h"
#include "components/ADC1/ADC1.h"
#include "components/ADCDispatcher/ADCDispatcher.h"
#include "components/BatteryCalculator/BatteryCalculator.h"
#include "components/BatteryCharger/BatteryCharger.h"
#include "components/BatteryStatusProvider/BatteryStatusProvider.h"
#include "components/BluetoothStatusObserver/BluetoothStatusObserver.h"
#include "components/CommunicationObserver/CommunicationObserver.h"
#include "components/DriveTrain/DriveTrain.h"
#include "components/ErrorStorage/ErrorStorage.h"
#include "components/ErrorStorageWrapper/ErrorStorageWrapper.h"
#include "components/GyroscopeOffsetCompensator/GyroscopeOffsetCompensator.h"
#include "components/HighResolutionTimer/HighResolutionTimer.h"
#include "components/IMU/IMU.h"
#include "components/IMUMovementDetector/IMUMovementDetector.h"
#include "components/IMUOrientationEstimator/IMUOrientationEstimator.h"
#include "components/InternalTemperatureSensor/InternalTemperatureSensor.h"
#include "components/LEDController/LEDController.h"
#include "components/LedDisplayController/LedDisplayController.h"
#include "components/MasterCommunication/MasterCommunication.h"
#include "components/MasterCommunicationInterface/MasterCommunicationInterface.h"
#include "components/MasterStatusObserver/MasterStatusObserver.h"
#include "components/McuStatusCollector/McuStatusCollector.h"
#include "components/McuStatusCollectorWrapper/McuStatusCollectorWrapper.h"
#include "components/MemoryAllocator/MemoryAllocator.h"
#include "components/MotorCurrentFilter/MotorCurrentFilter.h"
#include "components/MotorDerating/MotorDerating.h"
#include "components/MotorDriver_8833/MotorDriver_8833.h"
#include "components/MotorPortHandler/MotorPortHandler.h"
#include "components/MotorThermalModel/MotorThermalModel.h"
#include "components/ProjectConfiguration/ProjectConfiguration.h"
#include "components/RestartManager/RestartManager.h"
#include "components/RingLedDisplay/RingLedDisplay.h"
#include "components/SensorPortHandler/SensorPortHandler.h"
#include "components/VersionProvider/VersionProvider.h"
#include "components/WatchdogFeeder/WatchdogFeeder.h"
#include "components/YawAngleTracker/YawAngleTracker.h"

void Runtime_Call_OnInit(void);
void Runtime_Call_1ms(void);
void Runtime_Call_10ms_offset0(void);
void Runtime_Call_10ms_offset1(void);
void Runtime_Call_10ms_offset2(void);
void Runtime_Call_10ms_offset3(void);
void Runtime_Call_10ms_offset4(void);
void Runtime_Call_10ms_offset5(void);
void Runtime_Call_10ms_offset6(void);
void Runtime_Call_10ms_offset7(void);
void Runtime_Call_10ms_offset8(void);
void Runtime_Call_10ms_offset9(void);
void Runtime_Call_20ms_offset0(void);
void Runtime_Call_20ms_offset1(void);
void Runtime_Call_20ms_offset2(void);
void Runtime_Call_20ms_offset3(void);
void Runtime_Call_20ms_offset4(void);
void Runtime_Call_20ms_offset5(void);
void Runtime_Call_20ms_offset6(void);
void Runtime_Call_20ms_offset7(void);
void Runtime_Call_20ms_offset8(void);
void Runtime_Call_20ms_offset9(void);
void Runtime_Call_20ms_offset10(void);
void Runtime_Call_20ms_offset11(void);
void Runtime_Call_20ms_offset12(void);
void Runtime_Call_20ms_offset13(void);
void Runtime_Call_20ms_offset14(void);
void Runtime_Call_20ms_offset15(void);
void Runtime_Call_20ms_offset16(void);
void Runtime_Call_20ms_offset17(void);
void Runtime_Call_20ms_offset18(void);
void Runtime_Call_20ms_offset19(void);
void Runtime_Call_100ms(void);

#endif /* GENERATED_RUNTIME_H */
