/******************************************************************************
 * @file     gpio.c
 * @brief    Driver for the GPIO ports for the SAMD21E17
 * @version  V1.0.0
 * @date     11 November 2025
 ******************************************************************************/

 /*  Section 23 on the datasheet
 ******************************************************************************/
#include <drivers/gpio.h>


volatile gpio* const  porta = &PORT_REGS->GROUP[0];
volatile gpio* const  portb = &PORT_REGS->GROUP[1];
