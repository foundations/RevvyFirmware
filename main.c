#include "driver_init.h"
#include "rrrc_hal.h"
#include "rrrc_worklogic.h"

#include <string.h>

int main(void)
{
    system_init();

    if (ERR_NONE == RRRC_Init())
    {
        vTaskStartScheduler();
    }

    configASSERT(0);
}

void assert_failed(const char *file, uint32_t line)
{
    ErrorInfo_t data = {
        .error_id = ERROR_ID_ASSERTION_FAILURE,
        .data = {0}
    };

    memcpy(&data.data[0], &line, sizeof(uint32_t));
    /* save as much from the end of the filename as possible */
    size_t len = strlen(file);
    size_t available = sizeof(data.data) - sizeof(uint32_t);
    if (len > available)
    {
        size_t start_idx = len - available;
        strncpy((char*) &data.data[4], &file[start_idx], available);
    }
    else
    {
        strncpy((char*) &data.data[4], file, available);
    }
    ErrorStorage_Run_Store(&data);

    __BKPT(1);
    while(1);
}

/* Cortex-M4 core handlers */
void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
    /* These are volatile to try and prevent the compiler/linker optimising them
    away as the variables never actually get used. If the debugger won't show the
    values of the variables, make them global by moving their declaration outside
    of this function. */
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    uint32_t lr; /* Link register. */
    uint32_t pc; /* Program counter. */
    uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];
    
    /* suppress warnings */
    (void) r0;
    (void) r1;
    (void) r2;
    (void) r3;
    (void) r12;
    
    /* log the most important registers */
    ErrorInfo_t data = {
        .error_id = ERROR_ID_HARD_FAULT
    };
    memcpy(&data.data[0], &pc, sizeof(uint32_t));
    memcpy(&data.data[4], &psr, sizeof(uint32_t));
    memcpy(&data.data[8], &lr, sizeof(uint32_t));
    ErrorStorage_Run_Store(&data);

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
void MemManage_Handler( void )
{
    while (1) {
        __BKPT(1);
    }
}
void BusFault_Handler( void )
{
    while (1) {
        __BKPT(1);
    }
}
void UsageFault_Handler( void )
{
    while (1) {
        __BKPT(1);
    }
}
void SVC_Handler( void )
{
    while (1) {
        __BKPT(1);
    }
}
void DebugMon_Handler( void )
{
    while (1) {
        __BKPT(1);
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    ErrorInfo_t data = {
        .error_id = ERROR_ID_STACK_OVERFLOW,
        .data = {0}
    };
    strncpy((char*) &data.data[0], pcTaskName, sizeof(data.data));
    ErrorStorage_Run_Store(&data);

    while (1) {
        __BKPT(1);
    }
}
