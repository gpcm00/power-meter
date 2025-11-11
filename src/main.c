#define USING_SAMD21E17A_MCU
#include <power_logger.h>

 __STATIC_FORCEINLINE void wait_cycles(volatile uint32_t cycles)
{
    while (cycles--);
}

#define TST_BLINKER_BIT     BIT(0)

void main (void)
{
    porta->PORT_DIR = TST_BLINKER_BIT;
    porta->PORT_OUT = 0;
    while (true) {
        wait_cycles(500000);
        porta->PORT_OUT ^= TST_BLINKER_BIT;
    }
}