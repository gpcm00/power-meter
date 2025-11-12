#define USING_SAMD21E17A_MCU
#include <power_logger.h>

#define LED_R               BIT(27)
#define LED_O               BIT(23)
#define LED_Y               BIT(21)
#define LED_G               BIT(15)

#define TST_BLINKER_BIT     (LED_R|LED_O|LED_Y|LED_G)

 __STATIC_FORCEINLINE void wait_cycles(volatile uint32_t cycles)
{
    while (cycles--);
}

void main (void)
{
    porta->PORT_DIRSET = TST_BLINKER_BIT;
    porta->PORT_OUTCLR = TST_BLINKER_BIT;
    while (true) {
        wait_cycles(500000);
        porta->PORT_OUTTGL = TST_BLINKER_BIT;
    }
}