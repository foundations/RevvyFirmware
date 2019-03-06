

#include "driver_init.h"
#include "rrrc_hal.h"
#include "rrrc_worklogic.h"

#define BIT00           0x00000001
#define BIT01           0x00000002
#define BIT02           0x00000004
#define BIT03           0x00000008
#define BIT04           0x00000010
#define BIT05           0x00000020
#define BIT06           0x00000040
#define BIT07           0x00000080
#define BIT08           0x00000100
#define BIT09           0x00000200
#define BIT10           0x00000400
#define BIT11           0x00000800
#define BIT12           0x00001000
#define BIT13           0x00002000
#define BIT14           0x00004000
#define BIT15           0x00008000
#define BIT16           0x00010000
#define BIT17           0x00020000
#define BIT18           0x00040000
#define BIT19           0x00080000
#define BIT20           0x00100000
#define BIT21           0x00200000
#define BIT22           0x00400000
#define BIT23           0x00800000
#define BIT24           0x01000000
#define BIT25           0x02000000
#define BIT26           0x04000000
#define BIT27           0x08000000
#define BIT28           0x10000000
#define BIT29           0x20000000
#define BIT30           0x40000000
#define BIT31           0x80000000


#define GPIO0_LED0      BIT17
#define GPIO0_LED0_PORT GPIO_PORTC
#define GPIO0_LED1      BIT10
#define GPIO0_LED1_PORT GPIO_PORTA

#define LEDSA           BIT10 | BIT11
#define LEDSB           BIT07 | BIT23 | BIT24 | BIT25 | BIT26 | BIT27 | BIT28 | BIT29
#define LEDSC           BIT16 | BIT17 | BIT18 | BIT21 | BIT25 | BIT26 | BIT28
#define LEDSD           BIT10 | BIT11 | BIT12

int main(void)
{

	/* Initialize the SAM system */
	system_init();

	/* Initialize the RRRC logic */
	RRRC_Init();

	/* start RRRC main procces */
	RRRC_ProcessLogic();


    
    _gpio_set_direction(GPIO_PORTA, BIT19, GPIO_DIRECTION_OUT);
    
    _gpio_set_direction(GPIO_PORTA, LEDSA, GPIO_DIRECTION_OUT);
    _gpio_set_direction(GPIO_PORTB, LEDSB, GPIO_DIRECTION_OUT);
    _gpio_set_direction(GPIO_PORTC, LEDSC, GPIO_DIRECTION_OUT);
    _gpio_set_direction(GPIO_PORTD, LEDSD, GPIO_DIRECTION_OUT);

	/* Replace with your application code */
	while (1) {
        
        _gpio_set_level(GPIO_PORTA, BIT19, 1);
        uint32_t delayVal = 80;
        while (delayVal != 0) {--delayVal;}
        _gpio_set_level(GPIO_PORTA, BIT19, 0);
        
        
        
        
        _gpio_set_level(GPIO_PORTA, LEDSA, 1);
        _gpio_set_level(GPIO_PORTB, LEDSB, 1);
        _gpio_set_level(GPIO_PORTC, LEDSC, 1);
        _gpio_set_level(GPIO_PORTD, LEDSD, 1);
        delayVal = 800000;
        while (delayVal != 0) {--delayVal;}
            
        _gpio_set_level(GPIO_PORTA, LEDSA, 0);
        _gpio_set_level(GPIO_PORTB, LEDSB, 0);
        _gpio_set_level(GPIO_PORTC, LEDSC, 0);
        _gpio_set_level(GPIO_PORTD, LEDSD, 0);
         delayVal = 800000;
         while (delayVal != 0) {--delayVal;}
	}
}
