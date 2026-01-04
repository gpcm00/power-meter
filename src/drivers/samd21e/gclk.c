#include <drivers/samd21e/gclk.h>

#define LEN(arr)            (sizeof(arr)/sizeof(arr[0]))
#define LAST_ELEM(arr)      (LEN(arr) - 1)

static uint8_t gclkMUX[] = {
    0x7, 0x6, 0x5, 0x4, 0x3, 0x2,
};

static uint8_t next_gclkMUX = LAST_ELEM(gclkMUX);

static int pop_gclkMUX()
{
    if (next_gclkMUX < 0) {
        return -1;
    }

    return (int)gclkMUX[next_gclkMUX--];
}

static int push_gclkMUX(int clk)
{
    if (clk > 0x7 || clk < 0x2) {
        return -1;
    }

    if (next_gclkMUX > LAST_ELEM(gclkMUX)) {
        return -1;
    }

    gclkMUX[++next_gclkMUX] = clk;

    return 0;
}

int setupClk(uint16_t id, uint32_t div, enum LOGIC_LEVEL divsel)
{
    int clk = pop_gclkMUX();
    if (clk < 0x2) {
        return -1;
    }

    GCLK_REGS->GCLK_GENDIV = GCLK_GENDIV_ID(clk) |
                             GCLK_GENDIV_DIV(div);

    GCLK_REGS->GCLK_GENCTRL = GCLK_GENCTRL_ID(clk) |
                              GCLK_GENCTRL_SRC_DFLL48M |
                              GCLK_GENCTRL_GENEN(LOGIC_HIGH) |
                              GCLK_GENCTRL_DIVSEL(divsel);

    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk);

    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_GEN(clk) |
                              GCLK_CLKCTRL_ID(id) |
                              GCLK_CLKCTRL_CLKEN(LOGIC_HIGH);
    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk);

    return clk;
}

int requestClk(uint32_t gen, uint16_t id)
{
    if (gen > 0x7) {
        return -1;
    }

    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_GEN(gen) |
                              GCLK_CLKCTRL_ID(id) |
                              GCLK_CLKCTRL_CLKEN(LOGIC_HIGH);
    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk);

    return 0;
}

int resetClk(int clk)
{
    if (push_gclkMUX(clk) < 0) {
        return -1;
    }

    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_GEN(clk) |
                              GCLK_CLKCTRL_CLKEN(LOGIC_LOW);
    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk);

    GCLK_REGS->GCLK_GENDIV = GCLK_GENDIV_ID(clk) |
                             GCLK_GENDIV_DIV(0);

    GCLK_REGS->GCLK_GENDIV = 0;

    return 0;
}
