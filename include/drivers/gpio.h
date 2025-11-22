/******************************************************************************
 * @file     gpio.h
 * @brief    Header file for the GPIO ports
 * @version  V1.0.0
 * @date     20. January 2021
 ******************************************************************************/
#ifndef DRIVER_GPIO_H
#define DRIVER_GPIO_H

#include <common.h>

#ifdef USING_SAMD21E17A_MCU
#include <drivers/samd21e/port.h>
#else
#error "Unknown architecture"
#endif

#endif
