#include <drivers/spi.h>
#include <drivers/gpio.h>

#include <drivers/samd21e17a/gclk.h>
#include <interrupts/samd21e17a/interrupts.h>

#define SERCOM_BASE     (0x42000800)
#define SERCOM_OFFS     (0x400)
#define SERCOM(n)       ((spi*)(SERCOM_BASE + SERCOM_OFFS*n))

#define AT_PA(n)        ((int8_t)(n))
// there is no need to define AT_PB because samd21e17a doesn't have port b
#define NA              ((int8_t)(-1))

#define NUM_OF_SERCOM   4

typedef void (*isr_handler)(void);

const int8_t port_table[][NUM_OF_SERCOM] = {
    // port to pad config map for each sercom
    // see IO mux section in the datasheet for more info
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

static uint8_t dopo_table[][3] = {
    // pad configuration for sercom
    // see ctrla register in the spi section for more info
    // DO, CLK, CS
    {   0,   1,  2},      // DOPO 0
    {   2,   3,  1},      // DOPO 1
    {   3,   1,  2},      // DOPO 2
    {   0,   3,  1},      // DOPO 3
};

static void spi_isr_handler(void)
{
    // TODO: implement interrupt to handle transactions automatically
    while(1);
}

__STATIC_INLINE void enableAPBCMsk(uint8_t bus)
{
#define APBCMASK_SERCOM_iPos    2   // sercom0 bit offset in APBCMASK register
    PM_REGS->PM_APBCMASK |= _UINT32_(BIT(bus + APBCMASK_SERCOM_iPos));
}

__STATIC_INLINE void setDataOut(uint32_t port, spi_config* config,
                                                uint8_t pmuxo, uint8_t pmuxe)
{
    if (config->TXEN) {
        porta->PORT_DIRSET = BIT(port);
        porta->PORT_PINCFG[port] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);
        if (port & 1) {
            porta->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXO_Msk;
            porta->PORT_PMUX[port>>1] |= pmuxo;
        } else {
            porta->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXE_Msk;
            porta->PORT_PMUX[port>>1] |= pmuxe;
        }
    }
}
__STATIC_INLINE void setDataIn(uint32_t port, spi_config* config,
                                                uint8_t pmuxo, uint8_t pmuxe)
{
    if (config->RXEN) {
        porta->PORT_DIRCLR = BIT(port);
        porta->PORT_PINCFG[port] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);
        if (port & 1) {
            porta->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXO_Msk;
            porta->PORT_PMUX[port>>1] |= pmuxo;
        } else {
            porta->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXE_Msk;
            porta->PORT_PMUX[port>>1] |= pmuxe;
        }
    }
}
__STATIC_INLINE void setClock(uint32_t port, enum SPI_TYPE type,
                                                uint8_t pmuxo, uint8_t pmuxe)
{
    if (type == SPI_SLAVE) {
        porta->PORT_DIRCLR = BIT(port);
    } else {
        porta->PORT_DIRCLR = BIT(port);
    }

    porta->PORT_PINCFG[port] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);

    if (port & 1) {
        porta->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXO_Msk;
        porta->PORT_PMUX[port>>1] |= pmuxo;
    } else {
        porta->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXE_Msk;
        porta->PORT_PMUX[port>>1] |= pmuxe;
    }
}

__STATIC_INLINE void setChipSelect(uint32_t port, spi_config* config,
                             uint8_t pmuxo, uint8_t pmuxe, enum SPI_TYPE type)
{
    if (config->CSEN) {
        if (type == SPI_SLAVE) {
            porta->PORT_DIRCLR = BIT(port);
        } else {
            porta->PORT_DIRCLR = BIT(port);
        }

        porta->PORT_PINCFG[port] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);
        if (port&1) {
            porta->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXO_Msk;
            porta->PORT_PMUX[port>>1] |= pmuxo;
        } else {
            porta->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXE_Msk;
            porta->PORT_PMUX[port>>1] |= pmuxe;
        }
    }
}

static void push_isr_handler(uint8_t bus, void (*isr_handler)(void))
{
#define CASE_SERCOM_ISR(n)                          \
    case n:                                         \
        push_SERCOM##n##_Handler(isr_handler);      \
        break

    switch (bus) {
        CASE_SERCOM_ISR(0);
        CASE_SERCOM_ISR(1);
        CASE_SERCOM_ISR(2);
        CASE_SERCOM_ISR(3);
        default:
            break;
    }
}
static int checkPinConfig(spi_config* config);
static void initGPIO(uint8_t bus, enum SPI_TYPE type, spi_config* config);

int spi_init(uint8_t bus, uint8_t mode, uint32_t div, enum SPI_TYPE type, void* extra)
{
    spi_config* config = (spi_config*)extra;
    if (bus >= NUM_OF_SERCOM || checkPinConfig(config)) {
        return -1;
    }

    spi* spi_bus = SERCOM(bus);

    // initialize clock
    uint16_t clk_id = GCLK_CLKCTRL_ID_SERCOM0_CORE_Val + bus;
    int clk = setupClk(clk_id, div, LOGIC_HIGH);
    if (clk < 0) {
        return -1;
    }

    enableAPBCMsk(bus);

    // config SPI register
    spi_bus->SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_SWRST(LOGIC_HIGH);
    while (!(spi_bus->SERCOM_SYNCBUSY & SERCOM_SPIM_SYNCBUSY_SWRST_Msk));

    spi_bus->SERCOM_CTRLA = ((type == SPI_MASTER)? SERCOM_SPIM_CTRLA_MODE_SPI_MASTER :
                                                   SERCOM_SPIM_CTRLA_MODE_SPI_SLAVE) |
                            SERCOM_SPIM_CTRLA_RUNSTDBY(LOGIC_HIGH) |
                            SERCOM_SPIM_CTRLA_DOPO(config->DOPO) |
                            SERCOM_SPIM_CTRLA_DIPO(config->DIPO) |
                            SERCOM_SPIM_CTRLA_FORM(0x0) |
                            SERCOM_SPIM_CTRLA_CPHA(mode) |
                            SERCOM_SPIM_CTRLA_DORD(LOGIC_LOW);

    spi_bus->SERCOM_CTRLB = ((type == SPI_MASTER)? SERCOM_SPIM_CTRLB_MSSEN(LOGIC_HIGH) :
                                                   SERCOM_SPIM_CTRLB_SSDE(LOGIC_HIGH)) |
                            SERCOM_SPIM_CTRLB_PLOADEN(LOGIC_LOW) |
                            SERCOM_SPIM_CTRLB_CHSIZE_8_BIT;


    push_isr_handler(bus, &spi_isr_handler);

    spi_bus->SERCOM_INTENSET = ((type == SPI_MASTER)? SERCOM_SPIM_INTENSET_TXC_Msk :
                                                      SERCOM_SPIM_INTENSET_RXC_Msk) |
                                SERCOM_SPIM_INTENSET_ERROR_Msk;

    if (config->RXEN) {
        spi_bus->SERCOM_CTRLB |= SERCOM_SPIM_CTRLB_RXEN(LOGIC_HIGH);
        while (!(spi_bus->SERCOM_SYNCBUSY & SERCOM_SPIM_SYNCBUSY_CTRLB_Msk));
    }

    spi_bus->SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE(LOGIC_HIGH);
    while (!(spi_bus->SERCOM_SYNCBUSY & SERCOM_SPIM_SYNCBUSY_ENABLE_Msk));

    // TODO: add DMA

    initGPIO(bus, type, config);
    return 0;
}

static int checkPinConfig(spi_config* config)
{
    uint8_t dopo = config->DOPO & N_BIT_MASK(2);
    uint8_t dipo = config->DIPO & N_BIT_MASK(2);
    uint8_t txen = config->TXEN;
    uint8_t rxen = config->RXEN;
    uint8_t csen = config->CSEN;

    // both RX and TX map to the same pin
    if (dopo_table[dopo][0] == dipo && rxen && txen) {
        return -1;
    }

    // both RX and CLK map to the same pin
    if (dopo_table[dopo][1] == dipo && rxen) {
        return -1;
    }

    // both RX and CS map to the same pin
    if (dopo_table[dopo][2] == dipo && rxen && csen) {
        return -1;
    }

    return 0;
}


static void initGPIO(uint8_t bus, enum SPI_TYPE type, spi_config* config)
{
    int pt_offs = (int)bus;
    uint8_t pmuxo = PORT_PMUX_PMUXO_C;
    uint8_t pmuxe = PORT_PMUX_PMUXE_C;
    uint8_t dopo = config->DOPO;
    if (config->altpin) {
        pt_offs += NUM_OF_SERCOM;   // second half od the port_table
        pmuxo = PORT_PMUX_PMUXO_D;
        pmuxe = PORT_PMUX_PMUXE_D;
    }

    // get SERCOMn[x] for each function
    uint8_t dataIn = config->DIPO;
    uint8_t dataOut = dopo_table[dopo][0];
    uint8_t clk = dopo_table[dopo][1];
    uint8_t cs = dopo_table[dopo][2];

    setDataIn(port_table[pt_offs][dataIn], config, pmuxo, pmuxe);
    setDataOut(port_table[pt_offs][dataOut], config, pmuxo, pmuxe);
    setClock(port_table[pt_offs][clk], type, pmuxo, pmuxe);
    setChipSelect(port_table[pt_offs][cs], config, pmuxo, pmuxe, type);
}
