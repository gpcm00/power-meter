#ifndef SPI_H
#define SPI_H


#include <common.h>

#ifdef USING_SAMD21E17A_MCU
typedef sercom_spim_registers_t spi;

struct spi_config {
    uint8_t altpin;
    uint8_t dma;
    uint8_t DOPO;
    uint8_t DIPO;
};

#else
#error "Unknown architecture"
#endif

enum SPI_TYPE {SPI_SLAVE, SPI_MASTER};


#endif