#ifndef SPI_H
#define SPI_H


#include <common.h>

#ifdef USING_SAMD21E17A_MCU
#include <drivers/samd21e/sercom_spi.h>
#else
#error "Unknown architecture"
#endif

enum SPI_TYPE {SPI_SLAVE, SPI_MASTER};

int spi_init(uint8_t bus, uint8_t mode, uint32_t div, enum SPI_TYPE type, void* extra);
int spi_write(uint8_t bus, uint8_t* buffer, uint32_t len);
int spi_read(uint8_t bus, uint8_t* buffer, uint32_t len);
int spi_xfer(uint8_t bus, uint8_t* tx_buffer, uint8_t* rx_buffer, uint32_t len);

#endif
