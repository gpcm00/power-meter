/*
 * ARMCC startup file for ATSAMD21J15A
 *
 * Copyright (c) 2025 Microchip Technology Inc. and its subsidiaries.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "samd21j15a.h"

// Check if DeviceVectors is defined and create a typedef to VECTOR_TABLE_Type
#ifndef DEVICE_VECTORS_DEFINE
    typedef DeviceVectors VECTOR_TABLE_Type; // Defines the toolchain agnostic type of the vector table type defined in the 'samd21j15a.h'

    #define DEVICE_VECTORS_DEFINE
#endif


/*---------------------------------------------------------------------------
  External References
 *---------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
extern uint32_t __STACK_SEAL;
#endif

extern __NO_RETURN int __PROGRAM_START(void);

/*---------------------------------------------------------------------------
  Internal References
 *---------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void);
            void Default_Handler(void); /* Default empty handler */

/*---------------------------------------------------------------------------
  Exception / Interrupt Handler
 *---------------------------------------------------------------------------*/

/* Cortex-M0PLUS core handlers */
void NonMaskableInt_Handler ( void ) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler    ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SVCall_Handler       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler      ( void ) __attribute__ ((weak, alias("Default_Handler")));

/* Peripherals handlers */
void PM_Handler           ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SYSCTRL_Handler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void WDT_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void EIC_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void NVMCTRL_Handler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void DMAC_Handler         ( void ) __attribute__ ((weak, alias("Default_Handler")));
void USB_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void EVSYS_Handler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SERCOM0_Handler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SERCOM1_Handler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SERCOM2_Handler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SERCOM3_Handler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SERCOM4_Handler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SERCOM5_Handler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TCC0_Handler         ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TCC1_Handler         ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TCC2_Handler         ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TC3_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TC4_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TC5_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TC6_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TC7_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void ADC_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void AC_Handler           ( void ) __attribute__ ((weak, alias("Default_Handler")));
void DAC_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void PTC_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));
void I2S_Handler          ( void ) __attribute__ ((weak, alias("Default_Handler")));


/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE;
const VECTOR_TABLE_Type __VECTOR_TABLE __VECTOR_TABLE_ATTRIBUTE = {
        .pvStack = (void*) (&__INITIAL_SP),       /*     Initial Stack Pointer */



        .pfnReset_Handler              = (void*) Reset_Handler,
        .pfnNonMaskableInt_Handler     = (void*) NonMaskableInt_Handler,
        .pfnHardFault_Handler          = (void*) HardFault_Handler,
        .pvReservedC12                 = (void*) (0UL), /* Reserved */
        .pvReservedC11                 = (void*) (0UL), /* Reserved */
        .pvReservedC10                 = (void*) (0UL), /* Reserved */
        .pvReservedC9                  = (void*) (0UL), /* Reserved */
        .pvReservedC8                  = (void*) (0UL), /* Reserved */
        .pvReservedC7                  = (void*) (0UL), /* Reserved */
        .pvReservedC6                  = (void*) (0UL), /* Reserved */
        .pfnSVCall_Handler             = (void*) SVCall_Handler,
        .pvReservedC4                  = (void*) (0UL), /* Reserved */
        .pvReservedC3                  = (void*) (0UL), /* Reserved */
        .pfnPendSV_Handler             = (void*) PendSV_Handler,
        .pfnSysTick_Handler            = (void*) SysTick_Handler,

        /* Configurable interrupts */
        .pfnPM_Handler                 = (void*) PM_Handler,     /* 0  Power Manager */
        .pfnSYSCTRL_Handler            = (void*) SYSCTRL_Handler, /* 1  System Control */
        .pfnWDT_Handler                = (void*) WDT_Handler,    /* 2  Watchdog Timer */
        .pfnRTC_Handler                = (void*) RTC_Handler,    /* 3  Real-Time Counter */
        .pfnEIC_Handler                = (void*) EIC_Handler,    /* 4  External Interrupt Controller */
        .pfnNVMCTRL_Handler            = (void*) NVMCTRL_Handler, /* 5  Non-Volatile Memory Controller */
        .pfnDMAC_Handler               = (void*) DMAC_Handler,   /* 6  Direct Memory Access Controller */
        .pfnUSB_Handler                = (void*) USB_Handler,    /* 7  Universal Serial Bus */
        .pfnEVSYS_Handler              = (void*) EVSYS_Handler,  /* 8  Event System Interface */
        .pfnSERCOM0_Handler            = (void*) SERCOM0_Handler, /* 9  Serial Communication Interface */
        .pfnSERCOM1_Handler            = (void*) SERCOM1_Handler, /* 10 Serial Communication Interface */
        .pfnSERCOM2_Handler            = (void*) SERCOM2_Handler, /* 11 Serial Communication Interface */
        .pfnSERCOM3_Handler            = (void*) SERCOM3_Handler, /* 12 Serial Communication Interface */
        .pfnSERCOM4_Handler            = (void*) SERCOM4_Handler, /* 13 Serial Communication Interface */
        .pfnSERCOM5_Handler            = (void*) SERCOM5_Handler, /* 14 Serial Communication Interface */
        .pfnTCC0_Handler               = (void*) TCC0_Handler,   /* 15 Timer Counter Control */
        .pfnTCC1_Handler               = (void*) TCC1_Handler,   /* 16 Timer Counter Control */
        .pfnTCC2_Handler               = (void*) TCC2_Handler,   /* 17 Timer Counter Control */
        .pfnTC3_Handler                = (void*) TC3_Handler,    /* 18 Basic Timer Counter */
        .pfnTC4_Handler                = (void*) TC4_Handler,    /* 19 Basic Timer Counter */
        .pfnTC5_Handler                = (void*) TC5_Handler,    /* 20 Basic Timer Counter */
        .pfnTC6_Handler                = (void*) TC6_Handler,    /* 21 Basic Timer Counter */
        .pfnTC7_Handler                = (void*) TC7_Handler,    /* 22 Basic Timer Counter */
        .pfnADC_Handler                = (void*) ADC_Handler,    /* 23 Analog Digital Converter */
        .pfnAC_Handler                 = (void*) AC_Handler,     /* 24 Analog Comparators */
        .pfnDAC_Handler                = (void*) DAC_Handler,    /* 25 Digital Analog Converter */
        .pfnPTC_Handler                = (void*) PTC_Handler,    /* 26 Peripheral Touch Controller */
        .pfnI2S_Handler                = (void*) I2S_Handler     /* 27 Inter-IC Sound Interface */
};

#if defined ( __GNUC__ )
    #pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void)
{
    #if defined USE_CMSIS_INIT
    SystemInit();           /* CMSIS System Initialization */
    #endif /* USE_CMSIS_INIT */

    /* Branch to main function */
    __PROGRAM_START();      /* Enter PreMain (C library entry point) */
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
    while (1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
#endif
