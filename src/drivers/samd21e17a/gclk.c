#include <drivers/samd21e17a/gclk.h>

static uint32_t next_gclkMUX = 0x2;

int setupClk(uint16_t id, uint32_t div, enum LOGIC_LEVEL divsel)
{
    if (next_gclkMUX > 0x7) {
        return -1;
    }

    GCLK_REGS->GCLK_GENDIV = GCLK_GENDIV_ID(next_gclkMUX) |
                             GCLK_GENDIV_DIV(div);

    GCLK_REGS->GCLK_GENCTRL = GCLK_GENCTRL_ID(next_gclkMUX) |
                              GCLK_GENCTRL_SRC_DFLL48M |
                              GCLK_GENCTRL_GENEN(LOGIC_HIGH) |
                              GCLK_GENCTRL_DIVSEL(divsel);

    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk);

    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_GEN(next_gclkMUX) |
                              GCLK_CLKCTRL_ID(id) |
                              GCLK_CLKCTRL_CLKEN(LOGIC_HIGH);
    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk);

    return next_gclkMUX++;
}