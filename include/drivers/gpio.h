#ifndef DRIVER_GPIO_H
#define DRIVER_GPIO_H

/*  Section 23
 *  https://onlinedocs.microchip.com/oxy/GUID-22527069-B4D6-49B9-BACC-3AF1C52EB48C-en-US-20/index.html  
 *******************************************************************************************************/

#include <common.h>

#ifdef USING_SAMD21E17A_MCU
typedef port_group_registers_t gpio;
#else
#error "Unknown architecture"
#endif

extern gpio* const porta;
extern gpio* const portb; 

#endif