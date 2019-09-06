#ifndef GENERATED_RUNTIME_H_
#define GENERATED_RUNTIME_H_

#include "components/ADC/ADC.h"
#include "components/BatteryCalculator/BatteryCalculator.h"
#include "components/BatteryCharger/BatteryCharger.h"
#include "components/BatteryIndicator/BatteryIndicator.h"
#include "components/BatteryStatusProvider/BatteryStatusProvider.h"
#include "components/BluetoothIndicator/BluetoothIndicator.h"
#include "components/BluetoothStatusObserver/BluetoothStatusObserver.h"
#include "components/BrainStatusIndicator/BrainStatusIndicator.h"
#include "components/CommunicationObserver/CommunicationObserver.h"
#include "components/DriveTrain/DriveTrain.h"
#include "components/ErrorStorage/ErrorStorage.h"
#include "components/ErrorStorageWrapper/ErrorStorageWrapper.h"
#include "components/GyroscopeOffsetCompensator/GyroscopeOffsetCompensator.h"
#include "components/HighResolutionTimer/HighResolutionTimer.h"
#include "components/IMU/IMU.h"
#include "components/InternalTemperatureSensor/InternalTemperatureSensor.h"
#include "components/LEDController/LEDController.h"
#include "components/MasterCommunication/MasterCommunication.h"
#include "components/MasterCommunicationInterface/MasterCommunicationInterface.h"
#include "components/MasterStatusObserver/MasterStatusObserver.h"
#include "components/McuStatusCollector/McuStatusCollector.h"
#include "components/McuStatusCollectorWrapper/McuStatusCollectorWrapper.h"
#include "components/MemoryAllocator/MemoryAllocator.h"
#include "components/MotorDerating/MotorDerating.h"
#include "components/MotorDriver_8833/MotorDriver_8833.h"
#include "components/MotorPortHandler/MotorPortHandler.h"
#include "components/MotorThermalModel/MotorThermalModel.h"
#include "components/RestartManager/RestartManager.h"
#include "components/RingLedDisplay/RingLedDisplay.h"
#include "components/SensorPortHandler/SensorPortHandler.h"
#include "components/VersionProvider/VersionProvider.h"
#include "components/YawAngleTracker/YawAngleTracker.h"

void RunnableGroup_OnInit(void);

#endif /* GENERATED_RUNTIME_H */
