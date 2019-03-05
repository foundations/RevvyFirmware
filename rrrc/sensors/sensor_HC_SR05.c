
#include "rrrc_sensor_base_func.h"
#include "sensor_HC_SR05.h"

uint32_t GetTimerCounter(p_hw_sensor_port_t hwport);

#define MAX_SENSOR_VALUES 1
static int start=0;
int32_t HC_SR05_Init(void* hw_port)
{
	int32_t result = ERR_NONE;
	p_hw_sensor_port_t sensport = hw_port;

	memset(sensport->lib_data, 0, SENSOR_PORT_LIBDATA);

	SensorPort_gpio1_set_dir(sensport, GPIO_DIRECTION_OUT);
	SensorPort_gpio1_set_state(sensport, 0);	

	return result;
}

uint32_t HC_SR05_get_value(void* hw_port, uint32_t* data, uint32_t max_size)
{
	uint32_t amount = 0;
	p_hw_sensor_port_t sensport = hw_port;
	if (hw_port && data && max_size && (max_size>=MAX_SENSOR_VALUES))
	{
		p_hc_sr05_data_t sens_data = sensport->lib_data;
		data[0] = sens_data->distanse;
		amount = 1;
	}
	return amount;
}

void HC_SR05_Thread(void* hw_port)
{
	p_hw_sensor_port_t sensport = hw_port;
	if (sensport == NULL)
		return;
	
// 	static uint32_t run = 0;
	SensorPort_gpio1_set_state(sensport, 1);
	delay_us(10);
	SensorPort_gpio1_set_state(sensport, 0);
	start=1;

	return;
}

#define US_PWM_TIMER_PRESCALER (256u)
#define US_PWM_TIMER_FREQ_MHz (12u)
uint32_t GetTimerCounter(p_hw_sensor_port_t hwport)
{
    int32_t distance = hri_tccount16_read_COUNT_reg(TC0);
    /* Distance calculation, 10 / 58 = sonic wave speed conversion coefficient (in mm) */
    distance = (distance * US_PWM_TIMER_PRESCALER * 10u) / (US_PWM_TIMER_FREQ_MHz * 58u);
    if ((5000 <= distance) || (10 >= distance))
    {
        distance = 0;
    }
    return distance;
}

void HC_SR05_gpio0_callback(void* hw_port, uint32_t data)
{

	p_hw_sensor_port_t sensport = hw_port;
	if (!start)
	{
        hri_tcc_write_CTRLA_reg(TC0, (TC_CTRLA_PRESCALER_DIV256));
		SensorPort_timer_start(sensport);
        
		start = 0;
	}else
	{
        hri_tc_write_CTRLB_CMD_bf(TC0, TC_CTRLBSET_CMD_READSYNC_Val);
        uint32_t val = hri_tccount16_read_COUNT_reg(TC0);
        /* Distance calculation, 10 / 58 = sonic wave speed conversion coefficient (in mm) */
        val = (val * US_PWM_TIMER_PRESCALER * 10u) / (US_PWM_TIMER_FREQ_MHz * 58u);
        if ((5000 <= val) || (10 >= val))
        {
            val = 0;
        }

        //uint32_t val =  GetTimerCounter(sensport);
        
		SensorPort_timer_stop(sensport);
        
		
        
		uint32_t dist = 0;
		p_hc_sr05_data_t sens_data = sensport->lib_data;
		sens_data->distanse = dist;
		start = 1;
	}
	return;
}

void HC_SR05_gpio1_callback(void* hw_port, uint32_t data)
{
	p_hw_sensor_port_t sensport = hw_port;

	return;
}