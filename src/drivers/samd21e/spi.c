#include <drivers/spi.h>
#include <drivers/gpio.h>

#include <drivers/samd21e/gclk.h>
#include <interrupts/samd21e/interrupts.h>

#define SERCOM_BASE     (0x42000800)
#define SERCOM_OFFS     (0x400)
#define SERCOM(n)       ((spi*)(SERCOM_BASE + SERCOM_OFFS*n))

#define AT_PA(n)        ((int8_t)(n))
// there is no need to define AT_PB because samd21e17a doesn't have port b
#define NA              ((int8_t)(-1))

#define NUM_OF_SERCOM   4

static const int8_t port_table[][NUM_OF_SERCOM] = {
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

static const uint8_t dopo_table[][3] = {
    // pad configuration for sercom
    // see ctrla register in the spi section for more info
    // DO, CLK, CS
    {   0,   1,  2},      // DOPO 0
    {   2,   3,  1},      // DOPO 1
    {   3,   1,  2},      // DOPO 2
    {   0,   3,  1},      // DOPO 3
};

static struct xfer_buffer {
    uint8_t* txData;
    uint8_t* rxData;
    uint32_t len;
    uint32_t next;
}  spi_buffer[] = {
        {0,0,0},        // sercom0
        {0,0,0},        // sercom1
        {0,0,0},        // sercom2
        {0,0,0},        // sercom3
};

/* low level register access are inlined */
// SPI init helper functions ------------------------------------------------------------
__STATIC_INLINE void enableAPBCMsk(uint8_t bus)
{
#define APBCMASK_SERCOM_iPos    2   // sercom0 bit offset in APBCMASK register
    PM_REGS->PM_APBCMASK |= _UINT32_(BIT(bus + APBCMASK_SERCOM_iPos));
}

__STATIC_INLINE void resetSPIRegisters(spi* spi_bus)
{
    spi_bus->SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_SWRST(LOGIC_HIGH);
    while (!(spi_bus->SERCOM_SYNCBUSY & SERCOM_SPIM_SYNCBUSY_SWRST_Msk));
}

__STATIC_INLINE void configSPIRegisters(spi* spi_bus, enum SPI_TYPE type,
                                            uint8_t mode, spi_config* config)
{
    spi_bus->SERCOM_CTRLA = ((type == SPI_MASTER)?
                                SERCOM_SPIM_CTRLA_MODE_SPI_MASTER :
                                SERCOM_SPIM_CTRLA_MODE_SPI_SLAVE) |
                            SERCOM_SPIM_CTRLA_RUNSTDBY(LOGIC_HIGH) |
                            SERCOM_SPIM_CTRLA_DOPO(config->DOPO) |
                            SERCOM_SPIM_CTRLA_DIPO(config->DIPO) |
                            SERCOM_SPIM_CTRLA_FORM(0x0) |
                            SERCOM_SPIM_CTRLA_CPHA(mode) |
                            SERCOM_SPIM_CTRLA_DORD(LOGIC_LOW);

    spi_bus->SERCOM_CTRLB = ((type == SPI_MASTER)?
                                SERCOM_SPIM_CTRLB_MSSEN(LOGIC_HIGH) :
                                SERCOM_SPIM_CTRLB_SSDE(LOGIC_HIGH)) |
                            SERCOM_SPIM_CTRLB_PLOADEN(LOGIC_LOW) |
                            SERCOM_SPIM_CTRLB_CHSIZE_8_BIT;
}

__STATIC_INLINE void enableSPIInterrupt(spi* spi_bus, spi_config* config)
{
    spi_bus->SERCOM_INTENSET = ( (config->RXEN) ?
                                    SERCOM_SPIM_INTENSET_RXC(LOGIC_HIGH) : 0 ) |
                               ( (config->TXEN) ?
                                    SERCOM_SPIM_INTENSET_DRE(LOGIC_HIGH) : 0 ) |
                                SERCOM_SPIM_INTENSET_ERROR_Msk;
}

__STATIC_INLINE void enableSPIReceiver(spi* spi_bus)
{
    spi_bus->SERCOM_CTRLB |= SERCOM_SPIM_CTRLB_RXEN(LOGIC_HIGH);
    while (!(spi_bus->SERCOM_SYNCBUSY & SERCOM_SPIM_SYNCBUSY_CTRLB_Msk));
}

__STATIC_INLINE void enableSPIBus(spi* spi_bus)
{
    spi_bus->SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE(LOGIC_HIGH);
    while (!(spi_bus->SERCOM_SYNCBUSY & SERCOM_SPIM_SYNCBUSY_ENABLE_Msk));
}

__STATIC_INLINE void enableDebugCtrl(spi* spi_bus)
{
    spi_bus->SERCOM_DBGCTRL = SERCOM_SPIM_DBGCTRL_DBGSTOP(LOGIC_HIGH);
}

// SPI transfers helper functions -------------------------------------------------------
__STATIC_INLINE uint8_t writeData(uint8_t bus, spi* spi_bus)
{
    uint32_t next = spi_buffer[bus].next;
    if (spi_bus->SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk) {
        spi_bus->SERCOM_DATA = _UINT32_(spi_buffer[bus].txData[next]);
        return 1;
    }

    return 0;
}

__STATIC_INLINE uint8_t readData(uint8_t bus, spi* spi_bus)
{
    uint32_t next = spi_buffer[bus].next;
    if (spi_bus->SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_RXC_Msk) {
        spi_buffer[bus].rxData[next] = _UINT8_(spi_bus->SERCOM_DATA);
        return 1;
    }

    return 0;
}

__STATIC_INLINE uint8_t errorFlag(spi* spi_bus)
{
    return spi_bus->SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_ERROR_Msk;
}

// GPIO config helper functions ---------------------------------------------------------
__STATIC_INLINE void setDataOut(uint32_t port, spi_config* config,
                                                uint8_t pmuxo, uint8_t pmuxe)
{
    if (config->TXEN) {
        PortA->PORT_DIRSET = BIT(port);
        PortA->PORT_PINCFG[port] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);
        if (port & 1) {
            PortA->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXO_Msk;
            PortA->PORT_PMUX[port>>1] |= pmuxo;
        } else {
            PortA->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXE_Msk;
            PortA->PORT_PMUX[port>>1] |= pmuxe;
        }
    }
}
__STATIC_INLINE void setDataIn(uint32_t port, spi_config* config,
                                                uint8_t pmuxo, uint8_t pmuxe)
{
    if (config->RXEN) {
        PortA->PORT_DIRCLR = BIT(port);
        PortA->PORT_PINCFG[port] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);
        if (port & 1) {
            PortA->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXO_Msk;
            PortA->PORT_PMUX[port>>1] |= pmuxo;
        } else {
            PortA->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXE_Msk;
            PortA->PORT_PMUX[port>>1] |= pmuxe;
        }
    }
}
__STATIC_INLINE void setClock(uint32_t port, enum SPI_TYPE type,
                                                uint8_t pmuxo, uint8_t pmuxe)
{
    if (type == SPI_SLAVE) {
        PortA->PORT_DIRCLR = BIT(port);
    } else {
        PortA->PORT_DIRSET = BIT(port);
    }

    PortA->PORT_PINCFG[port] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);

    if (port & 1) {
        PortA->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXO_Msk;
        PortA->PORT_PMUX[port>>1] |= pmuxo;
    } else {
        PortA->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXE_Msk;
        PortA->PORT_PMUX[port>>1] |= pmuxe;
    }
}

__STATIC_INLINE void setChipSelect(uint32_t port, spi_config* config,
                             uint8_t pmuxo, uint8_t pmuxe, enum SPI_TYPE type)
{
    if (config->CSEN) {
        if (type == SPI_SLAVE) {
            PortA->PORT_DIRCLR = BIT(port);
        } else {
            PortA->PORT_DIRSET = BIT(port);
        }

        PortA->PORT_PINCFG[port] |= PORT_PINCFG_PMUXEN(LOGIC_HIGH);
        if (port & 1) {
            PortA->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXO_Msk;
            PortA->PORT_PMUX[port>>1] |= pmuxo;
        } else {
            PortA->PORT_PMUX[port>>1] &= ~PORT_PMUX_PMUXE_Msk;
            PortA->PORT_PMUX[port>>1] |= pmuxe;
        }
    }
}

static void pushISRHandler(uint8_t bus, void (*isr_handler)(uint8_t));
static int checkPinConfig(spi_config* config);
static void initGPIO(uint8_t bus, enum SPI_TYPE type, spi_config* config);

static void spi_isr_handler(uint8_t bus);

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

    enableAPBCMsk(bus);     // enable bus clock

    //configure registers
    resetSPIRegisters(spi_bus);
    configSPIRegisters(spi_bus, type, mode, config);

#ifdef DEBUG_MODE
    enableDebugCtrl(spi_bus);
#endif

    // transfer to xfer buffers are handled inside isr
    pushISRHandler(bus, &spi_isr_handler);
    enableSPIInterrupt(spi_bus, config);

    // if spi receive data
    if (config->RXEN) {
        enableSPIReceiver(spi_bus);
    }

    // start the bus
    enableSPIBus(spi_bus);

    // TODO: add DMA

    // config ports to operate in SPI mode
    initGPIO(bus, type, config);
    return 0;
}

int spi_write(uint8_t bus, uint8_t* buffer, uint32_t len)
{
    // check that bus is within limits and previous transfer is done
    if (bus >= NUM_OF_SERCOM || spi_buffer[bus].len > 0) {
        return -1;
    }
    spi* spi_bus = SERCOM(bus);

    spi_buffer[bus].txData = buffer;
    spi_buffer[bus].len = len;
    spi_buffer[bus].next = 0;

    if (writeData(bus, spi_bus)) {
        spi_buffer[bus].len--;
        spi_buffer[bus].next++;
    }

    return 0;
}

int spi_read(uint8_t bus, uint8_t* buffer, uint32_t len)
{
    // check that bus is within limits and previous transfer is done
    if (bus >= NUM_OF_SERCOM || spi_buffer[bus].len > 0) {
        return -1;
    }
    spi_buffer[bus].rxData = buffer;
    spi_buffer[bus].len = len;
    spi_buffer[bus].next = 0;

    return 0;
}

int spi_xfer(uint8_t bus, uint8_t* tx_buffer, uint8_t* rx_buffer, uint32_t len)
{
    // check that bus is within limits and previous transfer is done
    if (bus >= NUM_OF_SERCOM || spi_buffer[bus].len > 0) {
        return -1;
    }

    spi* spi_bus = SERCOM(bus);

    spi_buffer[bus].txData = tx_buffer;
    spi_buffer[bus].rxData = rx_buffer;
    spi_buffer[bus].len = len;
    spi_buffer[bus].next = 0;

    if (writeData(bus, spi_bus)) {
        spi_buffer[bus].len--;
        spi_buffer[bus].next++;
    }

    return 0;
}

static void pushISRHandler(uint8_t bus, void (*isr_handler)(uint8_t))
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

static int checkPinConfig(spi_config* config)
{
    uint8_t dopo = config->DOPO & BIT_MASK(2);
    uint8_t dipo = config->DIPO & BIT_MASK(2);
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


static void spi_isr_handler(uint8_t bus)
{
    spi* spi_bus = SERCOM(bus);

    // TODO: handle error
    if (errorFlag(spi_bus)) {
        while (1);
    }

    if (writeData(bus, spi_bus) || readData(bus, spi_bus)) {
        spi_buffer[bus].len--;
        spi_buffer[bus].next++;
    }

    if (spi_buffer[bus].len <= 0) {
        // TODO: add xfer done
        spi_buffer[bus].next = 0;               // avoid buffer overflow
        PortIOA->PORT_OUTTGL = BIT(22);         // dummy toggle for testing
    }
}
