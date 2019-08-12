/*
 * IMU.c
 *
 * Created: 2019. 07. 25. 16:31:24
 *  Author: Dániel Buga
 */ 

#include "IMU.h"
#include "imu_ll.h"

#define IMU_REGISTER_WHOAMI  ((uint8_t) 0x0Fu)
#define IMU_ACCEL_REGISTERS  ((uint8_t) 0x28u)
#define IMU_GYRO_REGISTERS   ((uint8_t) 0x22u)

#define IMU_AXL_LSB     ((float) 0.06103f)
#define IMU_GYRO_LSB    ((float) 0.007477f)

static bool imu_enabled;

typedef struct 
{
    int16_t x;
    int16_t y;
    int16_t z;
} imu_raw_data_t;

void IMU_Run_OnInit(void)
{
    imu_enabled = false;
    IMU_Call_LowLevelInit();
}

static void _send_configuration(void)
{
    /* TODO */
}

static void _read_data(uint8_t addr, imu_raw_data_t* data)
{
    uint8_t regs[6];
    _imu_read_registers(addr, regs, sizeof(regs));
    
    data->x = regs[0] | (regs[1] << 8u);
    data->y = regs[2] | (regs[3] << 8u);
    data->z = regs[4] | (regs[5] << 8u);
}

void IMU_Run_OnUpdate(void)
{
    if (!imu_enabled)
    {
        uint8_t whoami = _imu_read_register(IMU_REGISTER_WHOAMI);

        if (whoami == 0x6Au)
        {
            /* todo configure */
            _send_configuration();
            imu_enabled = true;
        }
    }
    else
    {
        if (imu_axl_data_ready())
        {
            imu_raw_data_t data;
            _read_data(IMU_ACCEL_REGISTERS, &data);

            IMU_AxlSample_t converted = {
                .x = data.x * IMU_AXL_LSB,
                .y = data.y * IMU_AXL_LSB,
                .z = data.z * IMU_AXL_LSB
            };
            IMU_Write_AccelerometerSample(&converted);
        }

        if (imu_gyro_data_ready())
        {
            imu_raw_data_t data;
            _read_data(IMU_GYRO_REGISTERS, &data);

            IMU_GyroSample_t converted = {
                .x = data.x * IMU_GYRO_LSB,
                .y = data.y * IMU_GYRO_LSB,
                .z = data.z * IMU_GYRO_LSB
            };
            IMU_Write_GyroscopeSample(&converted);
        }
    }
}

__attribute__((weak))
void IMU_Write_AccelerometerSample(const IMU_AxlSample_t* sample)
{
    (void) sample;
    /* nothing to do here */
}

__attribute__((weak))
void IMU_Write_GyroscopeSample(const IMU_GyroSample_t* sample)
{
    (void) sample;
    /* nothing to do here */
}
