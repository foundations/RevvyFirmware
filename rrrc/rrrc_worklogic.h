/*
 * rrrc_worklogic.h
 *
 * Created: 2/14/2019 11:38:39 AM
 *  Author: User
 */ 


#ifndef RRRC_WORKLOGIC_H_
#define RRRC_WORKLOGIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ERROR_ID_HARD_FAULT        (0u)
#define ERROR_ID_STACK_OVERFLOW    (1u)
#define ERROR_ID_ASSERTION_FAILURE (2u)
#define ERROR_ID_TEST_ERROR        (3u)

#include "components/ErrorStorage/ErrorStorage.h"
#include "components/ErrorStorageWrapper/ErrorStorageWrapper.h"

#include "components/ADC/adc.h"
#include "components/BatteryCharger/BatteryCharger.h"
#include "components/InternalTemperatureSensor/InternalTemperatureSensor.h"
#include "components/LEDController/LEDController.h"
#include "components/BluetoothIndicator/BluetoothIndicator.h"
#include "components/BrainStatusIndicator/BrainStatusIndicator.h"
#include "components/BatteryCalculator/BatteryCalculator.h"
#include "components/BatteryIndicator/BatteryIndicator.h"
#include "components/RingLedDisplay/RingLedDisplay.h"
#include "components/MasterCommunicationInterface/MasterCommunicationInterface.h"
#include "components/MasterCommunication/MasterCommunication.h"
#include "components/CommunicationObserver/CommunicationObserver.h"
#include "components/MasterStatusObserver/MasterStatusObserver.h"
#include "components/BluetoothStatusObserver/BluetoothStatusObserver.h"
#include "components/VersionProvider/VersionProvider.h"
#include "components/BatteryStatusProvider/BatteryStatusProvider.h"
#include "components/MotorPortHandler/MotorPortHandler.h"
#include "components/SensorPortHandler/SensorPortHandler.h"
#include "components/MemoryAllocator/MemoryAllocator.h"
#include "components/Drivetrain/Drivetrain.h"
#include "components/McuStatusCollector/McuStatusCollector.h"
#include "components/McuStatusCollectorWrapper/McuStatusCollectorWrapper.h"
#include "components/RestartManager/RestartManager.h"
#include "components/MotorDriver_8833/MotorDriver_8833.h"
#include "components/IMU/IMU.h"
#include "components/MotorThermalModel/MotorThermalModel.h"
#include "components/MotorDerating/MotorDerating.h"
#include "components/YawAngleTracker/YawAngleTracker.h"

int32_t RRRC_Init(void);
int32_t RRRC_DeInit(void);

#define COMM_HANDLER_COUNT  ((uint8_t) 0x41u)
const Comm_CommandHandler_t communicationHandlers[COMM_HANDLER_COUNT];

#ifdef __cplusplus
}
#endif

#endif /* RRRC_WORKLOGIC_H_ */

