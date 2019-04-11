/*
 * rrrc_worklogic.c
 *
 * Created: 2/14/2019 11:38:22 AM
 *  Author: User
 */ 

#include "rrrc_hal.h"
#include "rrrc_worklogic.h"
#include "rrrc_i2c_protocol.h"
#include "rrrc_sensors.h"
#include "rrrc_motors.h"
#include "rrrc_indication.h"
#include "rrrc_sysmon.h"

#include "jscope/jscope.h"

static TaskHandle_t xRRRC_Main_xTask;

extern hw_motor_port_t motor_ports[MOTOR_PORT_AMOUNT];
extern hw_sensor_port_t sensor_ports[SENSOR_PORT_AMOUNT];

void RRRC_ProcessLogic_xTask(void* user_data);

//*********************************************************************************************
static void SensorsPinsInit()
{
    //led pins
    for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
    {
        gpio_set_pin_pull_mode(sensor_ports[idx].led0_gpio, GPIO_PULL_UP);
        gpio_set_pin_function(sensor_ports[idx].led0_gpio, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(sensor_ports[idx].led0_gpio, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(sensor_ports[idx].led0_gpio, false);

        gpio_set_pin_pull_mode(sensor_ports[idx].led1_gpio, GPIO_PULL_UP);
        gpio_set_pin_function(sensor_ports[idx].led1_gpio, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(sensor_ports[idx].led1_gpio, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(sensor_ports[idx].led1_gpio, false);
    }

    //gpio0 - in & extint
    for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
    {
        gpio_set_pin_pull_mode(sensor_ports[idx].gpio0_num, GPIO_PULL_OFF);
        gpio_set_pin_direction(sensor_ports[idx].gpio0_num, GPIO_DIRECTION_IN);
        gpio_set_pin_function(sensor_ports[idx].gpio0_num, GPIO_PIN_FUNCTION_A);        
    }

    //gpio1 - out
    for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
    {
        gpio_set_pin_pull_mode(sensor_ports[idx].gpio1_num, GPIO_PULL_OFF);
        gpio_set_pin_direction(sensor_ports[idx].gpio1_num, GPIO_DIRECTION_OUT);
        gpio_set_pin_function(sensor_ports[idx].gpio1_num, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_level(sensor_ports[idx].gpio1_num, false);
    }

    //adc pins
    for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
    {
        gpio_set_pin_direction(sensor_ports[idx].adc_gpio, GPIO_DIRECTION_OFF);
        gpio_set_pin_function(sensor_ports[idx].adc_gpio, GPIO_PIN_FUNCTION_B);
    }

    // i2c pins - TODO now this pins init in driver_init()
//     for  (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
//     {
//         gpio_set_pin_pull_mode(sensor_ports[idx].i2c_gpio0, GPIO_PULL_OFF);
//         gpio_set_pin_direction(sensor_ports[idx].i2c_gpio0, GPIO_DIRECTION_OFF);
//         gpio_set_pin_function(sensor_ports[idx].i2c_gpio0, GPIO_PIN_FUNCTION_C);
//         gpio_set_pin_pull_mode(sensor_ports[idx].i2c_gpio1, GPIO_PULL_OFF);
//         gpio_set_pin_direction(sensor_ports[idx].i2c_gpio1, GPIO_DIRECTION_OFF);
//         gpio_set_pin_function(sensor_ports[idx].i2c_gpio1, GPIO_PIN_FUNCTION_C);
//     }

    for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++) 
    {
        gpio_set_pin_pull_mode(sensor_ports[idx].vccio_pin, GPIO_PULL_UP);
        gpio_set_pin_function(sensor_ports[idx].vccio_pin, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(sensor_ports[idx].vccio_pin, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(sensor_ports[idx].vccio_pin, false);//VCCIO_3V3
    }
}

//*********************************************************************************************
static void MotorsPinsInit()
{
    //led pins
    for (int idx=0; idx<ARRAY_SIZE(motor_ports); idx++)
    {
        gpio_set_pin_pull_mode(motor_ports[idx].led0_gpio, GPIO_PULL_UP);
        gpio_set_pin_function(motor_ports[idx].led0_gpio, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(motor_ports[idx].led0_gpio, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(motor_ports[idx].led0_gpio, false);

        gpio_set_pin_pull_mode(motor_ports[idx].led1_gpio, GPIO_PULL_UP);
        gpio_set_pin_function(motor_ports[idx].led1_gpio, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(motor_ports[idx].led1_gpio, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(motor_ports[idx].led1_gpio, false);
    }

    for (int idx=0; idx<ARRAY_SIZE(motor_ports); idx++)
    {
        tb6612fng_one_time_init(&motor_ports[idx]);

        // PWM
        gpio_set_pin_direction(motor_ports[idx].pwm_pin, GPIO_DIRECTION_OFF);
        gpio_set_pin_function(motor_ports[idx].pwm_pin, GPIO_PIN_FUNCTION_F);
    }

    uint8_t moto_stbypins[] = {M12STBY, M34STBY, M56STBY};
    for (int idx=0; idx<ARRAY_SIZE(moto_stbypins); idx++) // chips always ON
    {
        gpio_set_pin_pull_mode(moto_stbypins[idx], GPIO_PULL_UP);
        gpio_set_pin_function(moto_stbypins[idx], GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(moto_stbypins[idx], GPIO_DIRECTION_OUT);
        gpio_set_pin_level(moto_stbypins[idx], true);
    }

    gpio_set_pin_pull_mode(MOTPWEN, GPIO_PULL_UP);
    gpio_set_pin_function(MOTPWEN, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(MOTPWEN, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(MOTPWEN, true);
}

//*********************************************************************************************
void SystemMonitorPinsInit(void)
{
    gpio_set_pin_direction(SM_BAT_EN, GPIO_DIRECTION_IN);
    gpio_set_pin_function(SM_BAT_EN, GPIO_PIN_FUNCTION_OFF);

    gpio_set_pin_direction(SM_BAT_CHG, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SM_BAT_CHG, GPIO_PULL_UP);
    gpio_set_pin_function(SM_BAT_CHG, GPIO_PIN_FUNCTION_OFF);

    gpio_set_pin_direction(SM_BAT_PG, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SM_BAT_PG, GPIO_PULL_UP);
    gpio_set_pin_function(SM_BAT_PG, GPIO_PIN_FUNCTION_OFF);

    gpio_set_pin_direction(SM_BAT_ISET2, GPIO_DIRECTION_OUT);
    gpio_set_pin_function(SM_BAT_ISET2, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_level(SM_BAT_ISET2, false); //100mA

    gpio_set_pin_direction(SM_MOT_CURRENT_FAULT, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SM_MOT_CURRENT_FAULT, GPIO_PULL_UP);
    gpio_set_pin_function(SM_MOT_CURRENT_FAULT, GPIO_PIN_FUNCTION_OFF);

    //adc pins
    gpio_set_pin_direction(SM_MOT_VOLTAGE, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(SM_MOT_VOLTAGE, GPIO_PIN_FUNCTION_B);

    gpio_set_pin_direction(SM_MOT_CURRENT, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(SM_MOT_CURRENT, GPIO_PIN_FUNCTION_B);

    gpio_set_pin_direction(SM_BAT_VOLTAGE, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(SM_BAT_VOLTAGE, GPIO_PIN_FUNCTION_B);
}

//*********************************************************************************************
int32_t RRRC_Init(void)
{
    int32_t result = ERR_NONE;

    jscope_init();

    MotorsPinsInit();
    SensorsPinsInit();
    SystemMonitorPinsInit();

    for (uint32_t idx=0; idx<SENSOR_PORT_AMOUNT; idx++ )
    {
        result = SensorPortInit(idx);
        if (result != ERR_NONE)
            return result;
    }

    for (uint32_t idx=0; idx<MOTOR_PORT_AMOUNT; idx++ )
    {
        result = MotorPortInit(idx);
        if (result != ERR_NONE)
            return result;
    }

    result = IndicationInit();

    result = SysMon_Init();

    result = RRRC_Comminicationc_Init();

    adc_convertion_start(0);
    adc_convertion_start(1);

//     if (pdPASS != xTaskCreate(RRRC_ProcessLogic_xTask, "RRRC_Main", 256 / sizeof(portSTACK_TYPE), NULL, tskIDLE_PRIORITY+1, &xRRRC_Main_xTask))
//         return ERR_FAILURE;
    
//     SensorPortSetType(0,SENSOR_HC_SR05);
//     SensorPortSetType(1,SENSOR_ANALOG_SWITCH);
//    IndicationSetRingType(RING_LED_PREDEF_4);

    return ERR_NONE;
}

int32_t RRRC_DeInit(void)
{
    RRRC_Comminicationc_DeInit();
    
    adc_convertion_stop(0);
    adc_convertion_stop(1);

    vTaskDelete(xRRRC_Main_xTask);

    for (uint32_t idx=0; idx<SENSOR_PORT_AMOUNT; idx++ )
        SensorPortDeInit(idx);
    for (uint32_t idx=0; idx<MOTOR_PORT_AMOUNT; idx++ )
        MotorPortDeInit(idx);

    IndicationDeInit();

    SysMon_DeInit();

    return ERR_NONE;
}

//*********************************************************************************************
void RRRC_ProcessLogic_xTask(void* user    )
{

    while (1)
    {
         os_sleep(200*rtos_get_ticks_in_ms());
    }

}

