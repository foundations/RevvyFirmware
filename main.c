

#include "driver_init.h"
#include "rrrc_hal.h"
#include "rrrc_worklogic.h"

int main(void)
{

	/* Initialize the SAM system */
	system_init();

	/* Initialize the RRRC logic */
	RRRC_Init();

	/* start RRRC main process */
	RRRC_ProcessLogic();

	/* Replace with your application code */
	while (1) {

	}
}


/* Cortex-M4 core handlers */
void NMI_Handler             ( void ){
	while (1) {
	}
}
void HardFault_Handler       ( void ){
	while (1) {
	}
}
void MemManage_Handler       ( void )
{
	while (1) {
	}
}
void BusFault_Handler        ( void )
{
	while (1) {
	}
}
void UsageFault_Handler      ( void )
{
	while (1) {
	}
}
void SVC_Handler             ( void )
{
	while (1) {
	}
}
void DebugMon_Handler        ( void )
{
	while (1) {
	}
}
void PendSV_Handler          ( void )
{
	while (1) {
	}
}
void SysTick_Handler         ( void )
{
	while (1) {
	}
}
/* Peripherals handlers */
void PM_Handler              ( void )
{
	while (1) {
	}
}
void MCLK_Handler            ( void )
{
	while (1) {
	}
}