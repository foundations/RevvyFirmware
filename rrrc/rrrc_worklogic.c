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

#include "components/ADC/adc.h"
#include "components/BatteryCharger/BatteryCharger.h"
#include "components/InternalTemperatureSensor/InternalTemperatureSensor.h"
#include "components/LEDController/LEDController.h"

#include <math.h>

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

    //result = IndicationInit();

    result = RRRC_Communication_Init();

    if (pdPASS != xTaskCreate(RRRC_ProcessLogic_xTask, "RRRC_Main", 1024u, NULL, tskIDLE_PRIORITY+1, &xRRRC_Main_xTask))
    {
        return ERR_FAILURE;
    }

//     SensorPortSetType(0,SENSOR_HC_SR05);
//     SensorPortSetType(1,SENSOR_ANALOG_SWITCH);
//    IndicationSetRingType(RING_LED_PREDEF_4);

    return ERR_NONE;
}

int32_t RRRC_DeInit(void)
{
    RRRC_Communication_DeInit();
    
    vTaskDelete(xRRRC_Main_xTask);

    for (uint32_t idx=0; idx<SENSOR_PORT_AMOUNT; idx++ )
        SensorPortDeInit(idx);
    for (uint32_t idx=0; idx<MOTOR_PORT_AMOUNT; idx++ )
        MotorPortDeInit(idx);

    //IndicationDeInit();

    return ERR_NONE;
}

static void ProcessTasks_10ms(void)
{
    ADC_Run_Update();
    BatteryCharger_Run_Update();
}

static void ProcessTasks_20ms(void)
{
    /* TODO: Different ports may be handled with a time offset from each other to decrease jitter */
    for (uint32_t i = 0u; i < MOTOR_PORT_AMOUNT; i++)
    {
        /* Critical section (temporarily) necessary because motor ports can be configured in a different thread */
        taskENTER_CRITICAL();
        MotorPort_Update(i);
        taskEXIT_CRITICAL();
    }

    LEDController_Run_Update();
}

static void ProcessTasks_100ms(void)
{

}

//*********************************************************************************************
void RRRC_ProcessLogic_xTask(void* user)
{
    ADC_Run_OnInit();
    BatteryCharger_Run_OnInit();
    LEDController_Run_OnInit();

    BatteryCharger_Run_EnableFastCharge();

    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (uint8_t cycleCounter = 0u;;)
    {
        ProcessTasks_10ms();
        
        if (cycleCounter % 2 == 0)
        {
            ProcessTasks_20ms();
        }
        if (cycleCounter == 9u)
        {
            ProcessTasks_100ms();
            cycleCounter = 0u;
        }
        else
        {
            cycleCounter++;
        }

        vTaskDelayUntil(&xLastWakeTime, rtos_ms_to_ticks(10));
    }
}

#define SYSMON_ADC_MOTOR_VOLTAGE   ADC_BUFFER_ADC1_04
#define SYSMON_ADC_MOTOR_CURRENT   ADC_BUFFER_ADC1_10
#define SYSMON_ADC_BATTERY_VOLTAGE ADC_BUFFER_ADC1_11
#define SYSMON_ADC_TEMPERATURE_P   ADC_BUFFER_ADC1_28
#define SYSMON_ADC_TEMPERATURE_C   ADC_BUFFER_ADC1_29

void ADC_Write_RawSamples_ADC0(uint16_t samples[4])
{
    for (uint32_t i = 0u; i < 4u; i++)
    {
        SensorPort_Adc_Update(i, samples[i]);
    }
}

void ADC_Write_Samples_ADC1(float samples[5])
{
    rrrc_sysmon_t sysmon;
    sysmon.systicks = xTaskGetTickCount();
    sysmon.motor_voltage   = (uint32_t) lroundf(samples[SYSMON_ADC_MOTOR_VOLTAGE] * (130.0f / 30.0f));
    sysmon.battery_voltage = (uint32_t) lroundf(samples[SYSMON_ADC_BATTERY_VOLTAGE] * (340.0f / 240.0f));
    sysmon.motor_current   = (uint32_t) lroundf(samples[SYSMON_ADC_MOTOR_CURRENT]);
    InternalTemperatureSensor_Run_Convert(samples[SYSMON_ADC_TEMPERATURE_P], samples[SYSMON_ADC_TEMPERATURE_C], &sysmon.temperature);

    SystemMonitor_Update(&sysmon);
}

rgb_t LEDController_Read_StatusLED(uint32_t led_idx)
{
    return (rgb_t) LED_GREEN;
}

rgb_t LEDController_Read_RingLED(uint32_t led_idx)
{
    return (rgb_t){0, 0, 0};
}
