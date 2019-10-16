#ifndef COMPONENT_MCU_STATUS_SLOTS_H_
#define COMPONENT_MCU_STATUS_SLOTS_H_

#ifndef COMPONENT_TYPES_MCU_STATUS_SLOTS_H_
#define COMPONENT_TYPES_MCU_STATUS_SLOTS_H_

#include <float.h>
#include <stdint.h>
#include <stdio.h>


typedef struct {
    uint8_t* bytes;
    size_t count;
} ByteArray_t;

typedef struct {
    ByteArray_t data;
    uint8_t version;
} SlotData_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} IMU_RawSample_t;

typedef enum {
    ChargerState_NotPluggedIn,
    ChargerState_Charging,
    ChargerState_Charged,
    ChargerState_Fault
} ChargerState_t;

#endif /* COMPONENT_TYPES_MCU_STATUS_SLOTS_H_ */

void McuStatusSlots_Run_Reset(void);
void McuStatusSlots_Run_Update(void);
void McuStatusSlots_Run_UpdateSensorPort(uint8_t port, ByteArray_t data);
void McuStatusSlots_Run_UpdateMotorPort(uint8_t port, ByteArray_t data);
void McuStatusSlots_Write_SlotData(uint32_t index, const SlotData_t value);
void McuStatusSlots_Read_Acceleration(IMU_RawSample_t* value);
void McuStatusSlots_Read_AngularSpeeds(IMU_RawSample_t* value);
uint8_t McuStatusSlots_Read_MainBatteryLevel(void);
ChargerState_t McuStatusSlots_Read_MainBatteryStatus(void);
uint8_t McuStatusSlots_Read_MotorBatteryLevel(void);
float McuStatusSlots_Read_YawAngle(void);

#endif /* COMPONENT_MCU_STATUS_SLOTS_H_ */
