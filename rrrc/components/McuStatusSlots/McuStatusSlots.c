#include "utils.h"
#include "utils_assert.h"
#include "McuStatusSlots.h"

/* Begin User Code Section: Declarations */
#include <string.h>

#define MAX_MOTOR_STATUS_SIZE 10
#define MAX_SENSOR_STATUS_SIZE 4

#define STATUS_SLOT_BATTERY ((uint8_t) 10u)
#define STATUS_SLOT_AXL     ((uint8_t) 11u)
#define STATUS_SLOT_GYRO    ((uint8_t) 12u)
#define STATUS_SLOT_YAW     ((uint8_t) 13u)

typedef struct {
    ByteArray_t array;
    const uint8_t size;
    uint8_t version;
} slot_t;

#define MOTOR_SLOT(x)  (x)
#define SENSOR_SLOT(x) (x + 6)

static uint8_t motor_status[6][MAX_MOTOR_STATUS_SIZE] = {0};
static uint8_t sensor_status[4][MAX_SENSOR_STATUS_SIZE] = {0};
static uint8_t battery_status[4];
static uint8_t axl_status[6];
static uint8_t gyro_status[6];
static uint8_t yaw_status[8];

static slot_t slots[14] = {
    { .array = { .bytes = motor_status[0], .count = 0u }, .size = ARRAY_SIZE(motor_status[0]), .version = 0u },
    { .array = { .bytes = motor_status[1], .count = 0u }, .size = ARRAY_SIZE(motor_status[1]), .version = 0u },
    { .array = { .bytes = motor_status[2], .count = 0u }, .size = ARRAY_SIZE(motor_status[2]), .version = 0u },
    { .array = { .bytes = motor_status[3], .count = 0u }, .size = ARRAY_SIZE(motor_status[3]), .version = 0u },
    { .array = { .bytes = motor_status[4], .count = 0u }, .size = ARRAY_SIZE(motor_status[4]), .version = 0u },
    { .array = { .bytes = motor_status[5], .count = 0u }, .size = ARRAY_SIZE(motor_status[5]), .version = 0u },

    { .array = { .bytes = sensor_status[0], .count = 0u }, .size = ARRAY_SIZE(sensor_status[0]), .version = 0u },
    { .array = { .bytes = sensor_status[1], .count = 0u }, .size = ARRAY_SIZE(sensor_status[1]), .version = 0u },
    { .array = { .bytes = sensor_status[2], .count = 0u }, .size = ARRAY_SIZE(sensor_status[2]), .version = 0u },
    { .array = { .bytes = sensor_status[3], .count = 0u }, .size = ARRAY_SIZE(sensor_status[3]), .version = 0u },

    { .array = { .bytes = battery_status, .count = 0u }, .size = ARRAY_SIZE(battery_status), .version = 0u },
    { .array = { .bytes = axl_status,     .count = 0u }, .size = ARRAY_SIZE(axl_status),     .version = 0u },
    { .array = { .bytes = gyro_status,    .count = 0u }, .size = ARRAY_SIZE(gyro_status),    .version = 0u },
    { .array = { .bytes = yaw_status,     .count = 0u }, .size = ARRAY_SIZE(yaw_status),     .version = 0u }
};

static bool compare_and_copy(uint8_t* pDst, const uint8_t* pSrc, size_t size)
{
    bool equal = true;
    for (uint8_t i = 0u; i < size; i++)
    {
        if (pSrc[i] != pDst[i])
        {
            pDst[i] = pSrc[i];
            equal = false;
        }
    }

    return equal;
}

static void update_slot(uint8_t index, const uint8_t* data, uint8_t data_size)
{
    slot_t* const slot = &slots[index];
    ASSERT(data_size <= slot->size);

    bool slot_changed = true;
    __disable_irq();
    if (data_size != slot->array.count)
    {
        memcpy(slot->array.bytes, data, data_size);
        slot->array.count = data_size;
    }
    else
    {
        slot_changed = !compare_and_copy(slot->array.bytes, data, data_size);
    }

    if (slot_changed)
    {
        slot->version++;
        McuStatusSlots_Write_SlotData(index, (const SlotData_t) {.data = slot->array, .version = slot->version});
    }
    __enable_irq();
}
/* End User Code Section: Declarations */

void McuStatusSlots_Run_Reset(void)
{
    /* Begin User Code Section: Reset Start */
    __disable_irq();
    for (size_t i = 0u; i < ARRAY_SIZE(slots); i++)
    {
        slots[i].array.count = 0u;
        slots[i].version = 0u;
    }
    __enable_irq();
    /* End User Code Section: Reset Start */
    /* Begin User Code Section: Reset End */

    /* End User Code Section: Reset End */
}

void McuStatusSlots_Run_Update(void)
{
    /* Begin User Code Section: Update Start */

    /* battery slot */
    {
        const uint8_t status[4] = {
            (uint8_t) McuStatusSlots_Read_MainBatteryStatus(),
            McuStatusSlots_Read_MainBatteryLevel(),
            0, // motor status
            McuStatusSlots_Read_MotorBatteryLevel()
        };

        update_slot(STATUS_SLOT_BATTERY, status, 4u);
    }
    /* IMU slots */
    // TODO Use processed/filtered values instead of raw
    {
        IMU_RawSample_t sample;
        McuStatusSlots_Read_Acceleration(&sample);

        update_slot(STATUS_SLOT_AXL, (const uint8_t*) &sample, sizeof(sample));
    }
    {
        IMU_RawSample_t sample;
        McuStatusSlots_Read_AngularSpeeds(&sample);

        update_slot(STATUS_SLOT_GYRO, (const uint8_t*) &sample, sizeof(sample));
    }
    /* yaw angle slot */
    {
        int32_t yaw[2] = {
            McuStatusSlots_Read_YawAngle(),
            0 // TODO Remove
        };

        update_slot(STATUS_SLOT_YAW, (const uint8_t*) &yaw[0], sizeof(yaw));
    }

    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

void McuStatusSlots_Run_UpdateSensorPort(uint8_t port, ByteArray_t data)
{
    /* Begin User Code Section: UpdateSensorPort Start */
    update_slot(SENSOR_SLOT(port), data.bytes, data.count);
    /* End User Code Section: UpdateSensorPort Start */
    /* Begin User Code Section: UpdateSensorPort End */

    /* End User Code Section: UpdateSensorPort End */
}

void McuStatusSlots_Run_UpdateMotorPort(uint8_t port, ByteArray_t data)
{
    /* Begin User Code Section: UpdateMotorPort Start */
    update_slot(MOTOR_SLOT(port), data.bytes, data.count);
    /* End User Code Section: UpdateMotorPort Start */
    /* Begin User Code Section: UpdateMotorPort End */

    /* End User Code Section: UpdateMotorPort End */
}

__attribute__((weak))
void McuStatusSlots_Write_SlotData(uint32_t index, const SlotData_t value)
{
    (void) value;
    (void) index;
    ASSERT(index < 14);
    /* Begin User Code Section: SlotData Start */

    /* End User Code Section: SlotData Start */
    /* Begin User Code Section: SlotData End */

    /* End User Code Section: SlotData End */
}

__attribute__((weak))
void McuStatusSlots_Read_Acceleration(IMU_RawSample_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: Acceleration Start */

    /* End User Code Section: Acceleration Start */
    *value = (IMU_RawSample_t) { .x = 0, .y = 0, .z = 0 };
    /* Begin User Code Section: Acceleration End */

    /* End User Code Section: Acceleration End */
}

__attribute__((weak))
void McuStatusSlots_Read_AngularSpeeds(IMU_RawSample_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: AngularSpeeds Start */

    /* End User Code Section: AngularSpeeds Start */
    *value = (IMU_RawSample_t) { .x = 0, .y = 0, .z = 0 };
    /* Begin User Code Section: AngularSpeeds End */

    /* End User Code Section: AngularSpeeds End */
}

__attribute__((weak))
uint8_t McuStatusSlots_Read_MainBatteryLevel(void)
{
    /* Begin User Code Section: MainBatteryLevel Start */

    /* End User Code Section: MainBatteryLevel Start */
    /* Begin User Code Section: MainBatteryLevel End */

    /* End User Code Section: MainBatteryLevel End */
    return 0u;
}

__attribute__((weak))
ChargerState_t McuStatusSlots_Read_MainBatteryStatus(void)
{
    /* Begin User Code Section: MainBatteryStatus Start */

    /* End User Code Section: MainBatteryStatus Start */
    /* Begin User Code Section: MainBatteryStatus End */

    /* End User Code Section: MainBatteryStatus End */
    return ChargerState_NotPluggedIn;
}

__attribute__((weak))
uint8_t McuStatusSlots_Read_MotorBatteryLevel(void)
{
    /* Begin User Code Section: MotorBatteryLevel Start */

    /* End User Code Section: MotorBatteryLevel Start */
    /* Begin User Code Section: MotorBatteryLevel End */

    /* End User Code Section: MotorBatteryLevel End */
    return 0u;
}

__attribute__((weak))
int32_t McuStatusSlots_Read_YawAngle(void)
{
    /* Begin User Code Section: YawAngle Start */

    /* End User Code Section: YawAngle Start */
    /* Begin User Code Section: YawAngle End */

    /* End User Code Section: YawAngle End */
    return 0;
}
