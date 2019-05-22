

#include "driver_init.h"
#include "rrrc_hal.h"
#include "rrrc_worklogic.h"

int main(void)
{
	system_init();

	if (ERR_NONE == RRRC_Init())
		vTaskStartScheduler(); //main loop

	while (1) {
		__BKPT(1);
	}
	
}


/* Cortex-M4 core handlers */
void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
/* These are volatile to try and prevent the compiler/linker optimising them
away as the variables never actually get used.  If the debugger won't show the
values of the variables, make them global my moving their declaration outside
of this function. */
volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; /* Link register. */
volatile uint32_t pc; /* Program counter. */
volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* When the following line is hit, the variables contain the register values. */
    __BKPT(1);
    for( ;; );
}

void NMI_Handler( void ){
	while (1) {
		__BKPT(1);
	}
}

__attribute__((naked))
void HardFault_Handler( void )
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
    );
}
void MemManage_Handler       ( void )
{
	while (1) {
		__BKPT(1);
	}
}
void BusFault_Handler        ( void )
{
	while (1) {
		__BKPT(1);
	}
}
void UsageFault_Handler ( void )
{
	while (1) {
		__BKPT(1);
	}
}
void SVC_Handler ( void )
{
	while (1) {
		__BKPT(1);
	}
}
void DebugMon_Handler ( void )
{
	while (1) {
		__BKPT(1);
	}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	while (1) {
		__BKPT(1);
	}
}
void vApplicationMallocFailedHook()
{
	while (1) {
		__BKPT(1);
	}
}
// void PendSV_Handler          ( void )
// {
// 	while (1) {
// 	}
// }
// void SysTick_Handler         ( void )
// {
// 	while (1) {
// 	}
// }
// /* Peripherals handlers */
// void PM_Handler              ( void )
// {
// 	while (1) {
// 	}
// }
// void MCLK_Handler            ( void )
// {
// 	while (1) {
// 	}
// }
