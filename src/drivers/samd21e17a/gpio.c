/******************************************************************************
 * @file     gpio.c
 * @brief    Driver for the GPIO ports for the SAMD21E17
 * @version  V1.0.0
 * @date     20. January 2021
 ******************************************************************************/
/*  Section 23
 *  https://onlinedocs.microchip.com/oxy/GUID-22527069-B4D6-49B9-BACC-3AF1C52EB48C-en-US-20/index.html  
 *******************************************************************************************************/
#include <drivers/gpio.h>


volatile gpio* const  porta = &PORT_REGS->GROUP[0];
volatile gpio* const  portb = &PORT_REGS->GROUP[1];
