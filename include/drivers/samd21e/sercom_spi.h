#ifndef SAMD21E_SERCOM_SPI_H
#define SAMD21E_SERCOM_SPI_H

#include <common.h>
#include <samd21e17a.h>
#include <system_samd21e17a.h>

typedef volatile sercom_spim_registers_t spi;

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

#endif
