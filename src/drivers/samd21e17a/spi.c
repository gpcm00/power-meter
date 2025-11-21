#include <drivers/spi.h>
#include <drivers/gpio.h>

#include <drivers/samd21e17a/gclk.h>

#define SERCOM_BASE     (0x42000800)
#define SERCOM_OFFS     (0x400)
#define SERCOM(n)       ((struct spi*)(SERCOM_BASE + SERCOM_OFFS*n))

#define AT_PA(n)        ((int8_t)(n))
// there is no need to define AT_PB because samd21e17a doesn't have port b 
#define NA              ((int8_t)(-1))

#define NUM_OF_SERCOM   4

const int8_t port_table[][NUM_OF_SERCOM] = {
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

static void setGPIODIR(int i, uint32_t port, enum SPI_TYPE type, uint8_t dopo);
static void initGPIO(uint8_t bus, uint8_t altpin, enum SPI_TYPE type, uint8_t dopo);


int spi_init(uint8_t bus, uint8_t mode, uint32_t div, enum SPI_TYPE type, void* extra) 
{
    if (bus >= NUM_OF_SERCOM || ((mode & N_BIT_MASK(2)) > 3)) {
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

    // config SPI register
    
    // TODO: add DMA
    initGPIO(bus, config->altpin, type, config->DOPO);
    return 0;

}


static void setGPIODIR(int i, uint32_t port, enum SPI_TYPE type, uint8_t dopo)
{
    switch (dopo & N_BIT_MASK(2))
    {
    case 0:
        // sercom[0] = DO, sercom[1] = CLK, sercom[2] = CS
        if (i == 0 || (type == SPI_MASTER && (i == 1 || i == 2))) {
            porta->PORT_DIRSET = BIT(port);
        } else {
            porta->PORT_DIRCLR = BIT(port);
        }
        
        break;
    
    case 1:
        // sercom[2] = DO, sercom[3] = CLK, sercom[1] = CS
        if (i == 2 || (type == SPI_MASTER && (i == 3 || i == 1))) {
            porta->PORT_DIRSET = BIT(port);
        } else {
            porta->PORT_DIRCLR = BIT(port);
        }
        break;
    
    case 2:
        // sercom[3] = DO, sercom[1] = CLK, sercom[2] = CS
        if (i == 3 || (type == SPI_MASTER && (i == 1 || i == 2))){
            porta->PORT_DIRSET = BIT(port);
        } else {
            porta->PORT_DIRCLR = BIT(port);
        }
        break;
    
    case 3:
        // sercom[0] = DO, sercom[3] = CLK, sercom[1] = CS
        if (i == 0 || (type == SPI_MASTER && (i == 3 || i == 1))){
            porta->PORT_DIRSET = BIT(port);
        } else {
            porta->PORT_DIRCLR = BIT(port);
        }
        break;
    
    default:
        break;
    }
}

static void initGPIO(uint8_t bus, uint8_t altpin, enum SPI_TYPE type, uint8_t dopo)
{
    int pt_offs = bus;
    uint8_t pmuxo = PORT_PMUX_PMUXO_C;
    uint8_t pmuxe = PORT_PMUX_PMUXE_C;
    if (altpin) {
        pt_offs += NUM_OF_SERCOM;
        pmuxo = PORT_PMUX_PMUXO_D;
        pmuxe = PORT_PMUX_PMUXE_D;
    }

    for (int i = 0; i < NUM_OF_SERCOM; i++) {
        uint32_t port = _UINT32_(port_table[pt_offs][i]);
        porta->PORT_PINCFG[port] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);
        porta->PORT_PMUX[port>>1] &= (port&1)? ~PORT_PMUX_PMUXO_Msk : ~PORT_PMUX_PMUXE_Msk;
        porta->PORT_PMUX[port>>1] |= (port&1)? pmuxo : pmuxe;

        setGPIODIR(i, port, type, dopo);
    }
}