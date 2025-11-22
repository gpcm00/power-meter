#ifndef USING_SAMD21E17A_MCU
#define USING_SAMD21E17A_MCU
#endif

#include <power_logger.h>

#define LED_R               (27)
#define LED_O               BIT(23)
#define LED_Y               BIT(22)
#define LED_G               (15)

#define TST_BLINKER_BIT     (LED_O|LED_Y)

 __STATIC_FORCEINLINE void wait_cycles(volatile uint32_t cycles)
{
    while (cycles--);
}

void main (void)
{
    PortA->PORT_DIRSET = TST_BLINKER_BIT;
    PortA->PORT_OUTCLR = TST_BLINKER_BIT;

    PortA->PORT_PINCFG[LED_G] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);
    PortA->PORT_PINCFG[LED_R] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);
    PortA->PORT_PMUX[LED_G>>1] = PORT_PMUX_PMUXO_H;
    PortA->PORT_PMUX[LED_R>>1] = PORT_PMUX_PMUXO_H;

    spi_config config = {
        .CSEN = LOGIC_HIGH,
        .DOPO = 3,
        .DIPO = 0,
        .TXEN = LOGIC_HIGH,
        .RXEN = LOGIC_LOW,
        .altpin = LOGIC_HIGH,
        .dma = LOGIC_HIGH,
    };

    char hello_world[] = "Hello SPI bus!";

    spi_init(0, 0, 4, SPI_MASTER, &config);
    spi_write(0, (uint8_t*)hello_world, sizeof(hello_world));
    while (1) {
        // wait_cycles(10);
        // PortA->PORT_OUTTGL = TST_BLINKER_BIT;
    }
}
