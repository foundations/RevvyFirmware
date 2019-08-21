/*
 * IMU.c
 *
 * Created: 2019. 07. 25. 16:31:24
 *  Author: Dániel Buga
 */ 

#include "IMU.h"
#include "imu_ll.h"

#include "imu_defs.h"

#define IMU_AXL_LSB     ((float) 0.061f)
#define IMU_GYRO_LSB    ((float) 0.035f)

static bool imu_enabled;

void IMU_Run_OnInit(void)
{
    imu_enabled = false;
    IMU_Call_LowLevelInit();
}

static void _send_configuration(void)
{
    _imu_write_register(LSM6DS3_REG(CTRL3_C), LSM6DS3_FIELD_ENABLE(SW_RESET));

    _imu_write_register(LSM6DS3_REG(INT1_CTRL), LSM6DS3_FIELD_ENABLE(INT1_DRDY_XL));
    _imu_write_register(LSM6DS3_REG(INT2_CTRL), LSM6DS3_FIELD_ENABLE(INT2_DRDY_G));

    _imu_write_register(LSM6DS3_REG(CTRL1_XL), LSM6DS3_FIELD_NAMED(FS_XL, 2g) | LSM6DS3_FIELD_NAMED(ODR_XL, 416));
    _imu_write_register(LSM6DS3_REG(CTRL2_G),  LSM6DS3_FIELD_NAMED(FS_G, 1000) | LSM6DS3_FIELD_NAMED(ODR_G, 416));
    
    _imu_write_register(LSM6DS3_REG(CTRL4_C),  LSM6DS3_FIELD_ENABLE(I2C_disable));
    
    _imu_write_register(LSM6DS3_REG(CTRL5_C), 0);
}

static void _read_data(uint8_t addr, IMU_RawSample_t* data)
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
        uint8_t whoami = _imu_read_register(LSM6DS3_REG(WHOAMI));

        if (whoami == LSM6DS3_WHOAMI_VALUE)
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
            IMU_RawSample_t data;
            _read_data(LSM6DS3_ACCEL_REGISTERS, &data);

            IMU_AxlSample_t converted = {
                .x = data.x * IMU_AXL_LSB,
                .y = data.y * IMU_AXL_LSB,
                .z = data.z * IMU_AXL_LSB
            };
            IMU_Write_RawAccelerometerSample(&data);
            IMU_Write_AccelerometerSample(&converted);
        }

        if (imu_gyro_data_ready())
        {
            IMU_RawSample_t data;
            _read_data(LSM6DS3_GYRO_REGISTERS, &data);

            IMU_GyroSample_t converted = {
                .x = data.x * IMU_GYRO_LSB,
                .y = data.y * IMU_GYRO_LSB,
                .z = data.z * IMU_GYRO_LSB
            };
            IMU_Write_RawGyroscopeSample(&data);
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

__attribute__((weak))
void IMU_Write_RawAccelerometerSample(const IMU_RawSample_t* sample)
{
    (void) sample;
    /* nothing to do here */
}

__attribute__((weak))
void IMU_Write_RawGyroscopeSample(const IMU_RawSample_t* sample)
{
    (void) sample;
    /* nothing to do here */
}
