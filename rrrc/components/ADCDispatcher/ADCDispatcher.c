#include "utils_assert.h"
#include "ADCDispatcher.h"
#include "utils.h"

/* Begin User Code Section: Declarations */
#include <math.h>

/* ADC0 */
#define S3_ADC_CH  ((uint32_t) 0u)
#define M1_ISEN_CH ((uint32_t) 1u)
#define M3_ISEN_CH ((uint32_t) 2u)
#define M4_ISEN_CH ((uint32_t) 3u)

/* ADC1 */
#define S2_ADC_CH          ((uint32_t) 0u)
#define S1_ADC_CH          ((uint32_t) 1u)
#define S0_ADC_CH          ((uint32_t) 2u)
#define M0_ISEN_CH         ((uint32_t) 3u)
#define M2_ISEN_CH         ((uint32_t) 4u)
#define M5_ISEN_CH         ((uint32_t) 5u)
#define ADC_CH_BAT_VOLTAGE ((uint32_t) 6u)
#define ADC_CH_MOT_VOLTAGE ((uint32_t) 7u)
/* End User Code Section: Declarations */

void ADCDispatcher_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    ADCDispatcher_Write_Sensor_ADC(0u, ADCDispatcher_Read_ADC1_RawChannelData(S0_ADC_CH) >> 4u);
    ADCDispatcher_Write_Sensor_ADC(1u, ADCDispatcher_Read_ADC1_RawChannelData(S1_ADC_CH) >> 4u);
    ADCDispatcher_Write_Sensor_ADC(2u, ADCDispatcher_Read_ADC1_RawChannelData(S2_ADC_CH) >> 4u);
    ADCDispatcher_Write_Sensor_ADC(3u, ADCDispatcher_Read_ADC0_RawChannelData(S3_ADC_CH) >> 4u);

    ADCDispatcher_Write_MotorBatteryVoltage((uint32_t) lroundf(ADCDispatcher_Read_ADC1_ChannelVoltage(ADC_CH_MOT_VOLTAGE) * (130.0f / 30.0f)));
    ADCDispatcher_Write_MainBatteryVoltage((uint32_t) lroundf(ADCDispatcher_Read_ADC1_ChannelVoltage(ADC_CH_BAT_VOLTAGE) * (340.0f / 240.0f)));

    /* remap to 'brain index' */
    ADCDispatcher_Write_MotorCurrent(3u, ADCDispatcher_Read_ADC1_ChannelVoltage(M0_ISEN_CH) / 120.0f); //mV / mOhm -> A
    ADCDispatcher_Write_MotorCurrent(4u, ADCDispatcher_Read_ADC0_ChannelVoltage(M1_ISEN_CH) / 120.0f); //mV / mOhm -> A
    ADCDispatcher_Write_MotorCurrent(5u, ADCDispatcher_Read_ADC1_ChannelVoltage(M2_ISEN_CH) / 120.0f); //mV / mOhm -> A
    ADCDispatcher_Write_MotorCurrent(2u, ADCDispatcher_Read_ADC0_ChannelVoltage(M3_ISEN_CH) / 120.0f); //mV / mOhm -> A
    ADCDispatcher_Write_MotorCurrent(1u, ADCDispatcher_Read_ADC0_ChannelVoltage(M4_ISEN_CH) / 120.0f); //mV / mOhm -> A
    ADCDispatcher_Write_MotorCurrent(0u, ADCDispatcher_Read_ADC1_ChannelVoltage(M5_ISEN_CH) / 120.0f); //mV / mOhm -> A
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
void ADCDispatcher_Write_MainBatteryVoltage(const Voltage_t value)
{
    (void) value;
    /* Begin User Code Section: MainBatteryVoltage Start */

    /* End User Code Section: MainBatteryVoltage Start */
    /* Begin User Code Section: MainBatteryVoltage End */

    /* End User Code Section: MainBatteryVoltage End */
}

__attribute__((weak))
void ADCDispatcher_Write_MotorBatteryVoltage(const Voltage_t value)
{
    (void) value;
    /* Begin User Code Section: MotorBatteryVoltage Start */

    /* End User Code Section: MotorBatteryVoltage Start */
    /* Begin User Code Section: MotorBatteryVoltage End */

    /* End User Code Section: MotorBatteryVoltage End */
}

__attribute__((weak))
void ADCDispatcher_Write_MotorCurrent(uint32_t index, const Current_t value)
{
    (void) value;
    (void) index;
    ASSERT(index < 6);
    /* Begin User Code Section: MotorCurrent Start */

    /* End User Code Section: MotorCurrent Start */
    /* Begin User Code Section: MotorCurrent End */

    /* End User Code Section: MotorCurrent End */
}

__attribute__((weak))
void ADCDispatcher_Write_Sensor_ADC(uint32_t index, const uint8_t value)
{
    (void) value;
    (void) index;
    ASSERT(index < 4);
    /* Begin User Code Section: Sensor_ADC Start */

    /* End User Code Section: Sensor_ADC Start */
    /* Begin User Code Section: Sensor_ADC End */

    /* End User Code Section: Sensor_ADC End */
}

__attribute__((weak))
Voltage_t ADCDispatcher_Read_ADC0_ChannelVoltage(uint32_t index)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADC0_ChannelVoltage Start */

    /* End User Code Section: ADC0_ChannelVoltage Start */
    /* Begin User Code Section: ADC0_ChannelVoltage End */

    /* End User Code Section: ADC0_ChannelVoltage End */
    return 0.0f;
}

__attribute__((weak))
uint16_t ADCDispatcher_Read_ADC0_RawChannelData(uint32_t index)
{
    ASSERT(index < 4);
    /* Begin User Code Section: ADC0_RawChannelData Start */

    /* End User Code Section: ADC0_RawChannelData Start */
    /* Begin User Code Section: ADC0_RawChannelData End */

    /* End User Code Section: ADC0_RawChannelData End */
    return 0u;
}

__attribute__((weak))
Voltage_t ADCDispatcher_Read_ADC1_ChannelVoltage(uint32_t index)
{
    ASSERT(index < 8);
    /* Begin User Code Section: ADC1_ChannelVoltage Start */

    /* End User Code Section: ADC1_ChannelVoltage Start */
    /* Begin User Code Section: ADC1_ChannelVoltage End */

    /* End User Code Section: ADC1_ChannelVoltage End */
    return 0.0f;
}

__attribute__((weak))
uint16_t ADCDispatcher_Read_ADC1_RawChannelData(uint32_t index)
{
    ASSERT(index < 8);
    /* Begin User Code Section: ADC1_RawChannelData Start */

    /* End User Code Section: ADC1_RawChannelData Start */
    /* Begin User Code Section: ADC1_RawChannelData End */

    /* End User Code Section: ADC1_RawChannelData End */
    return 0u;
}
