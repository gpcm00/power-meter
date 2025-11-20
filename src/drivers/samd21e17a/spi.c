#include <drivers/spi.h>
#include <drivers/gpio.h>

#include <drivers/samd21e17a/gclk.h>

#define SERCOM_BASE     (0x42000800)
#define SERCOM_OFFS     (0x400)
#define SERCOM(n)       ((struct spi*)(SERCOM_BASE + n * SERCOM_OFFS))

#define AT_PA(n)        ((int8_t)(n))
/*#define AT_PB(n)        ((int8_t)(~(n))) /* samd21e17a doesn't have pb */
#define NA              ((int8_t)(0x80))

const int8_t port_table[][4] = {
    {AT_PA( 8), AT_PA( 9), AT_PA(10), AT_PA(11)},   // SERCOM0
    {AT_PA(16), AT_PA(17), AT_PA(18), AT_PA(19)},   // SERCOM1
    {AT_PA(NA), AT_PA(NA), AT_PA(14), AT_PA(15)},   // SERCOM2
    {AT_PA(22), AT_PA(23), AT_PA(24), AT_PA(25)},   // SERCOM3

    // alternative pins
    {AT_PA( 4), AT_PA( 5), AT_PA( 6), AT_PA( 7)},   // SERCOM0
    {AT_PA( 0), AT_PA( 1), AT_PA(30), AT_PA(31)},   // SERCOM1
    {AT_PA( 8), AT_PA( 9), AT_PA(10), AT_PA(11)},   // SERCOM2
    {AT_PA(16), AT_PA(17), AT_PA(18), AT_PA(19)},   // SERCOM3
};

__STATIC_INLINE void enableAPBCMASK(uint8_t bus)
{
#define APBCMASK_SERCOM_iPos    2   // sercom0 bit offset in APBCMASK register
    PM_REGS->PM_APBCMASK |= _UINT32_(BIT(bus + APBCMASK_SERCOM_iPos));
}

__STATIC_INLINE void initGPIO(uint8_t bus, uint8_t altpin)
{
    int pt_offs = 0;
    uint8_t pmuxo = PORT_PMUX_PMUXO_C;
    uint8_t pmuxe = PORT_PMUX_PMUXE_C;
    if (altpin) {
        pt_offs = 0;
        pmuxo = PORT_PMUX_PMUXO_D;
        pmuxe = PORT_PMUX_PMUXE_D;
    }
    
    for (int i = bus + pt_offs; i < bus+4; i++) {
        porta->PORT_PINCFG[i] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);
        porta->PORT_PMUX[i>>1] &= (i&1)? ~PORT_PMUX_PMUXO_Msk : PORT_PMUX_PMUXE_Msk;
        porta->PORT_PMUX[i>>1] |= (i&1)? pmuxo : pmuxe;

        // add dopo and dipo logic for input or output
    }
}

int spi_init(uint8_t bus, uint8_t mode, uint32_t div, void* extra) 
{
    if (bus > 3 || mode > 3) {
        return -1;
    }

    struct spi* spi_bus = SERCOM(bus);
    struct spi_config* config = (struct spi_config*)extra;

    // initialize clock
    uint16_t clk_id = GCLK_CLKCTRL_ID_SERCOM0_CORE_Val + bus;
    int clk = setupClk(clk_id, div, LOGIC_HIGH);
    if (clk < 0) {
        return -1;
    }

    enableAPBCMASK(bus);
    
    // TODO: add DMA
    initGPIO(bus, config->altpin);
    return 0;

}
