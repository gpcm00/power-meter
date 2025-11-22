#ifndef SPI_H
#define SPI_H


#include <common.h>

#ifdef USING_SAMD21E17A_MCU
typedef sercom_spim_registers_t spi;

typedef struct spi_config {
    uint8_t altpin;
    uint8_t dma;

    // register specific
    uint8_t RXEN;
    uint8_t TXEN;
    uint8_t CSEN;
    uint8_t DOPO;
    uint8_t DIPO;
} spi_config;

#else
#error "Unknown architecture"
#endif

enum SPI_TYPE {SPI_SLAVE, SPI_MASTER};
int spi_init(uint8_t bus, uint8_t mode, uint32_t div, enum SPI_TYPE type, void* extra);

#endif
