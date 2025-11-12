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
typedef port_group_registers_t gpio;
#else
#error "Unknown architecture"
#endif

extern gpio* const porta;
extern gpio* const portb; 

#endif