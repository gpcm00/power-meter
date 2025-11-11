/******************************************************************************
 * @file     startup_samd21e17a.c
 * @brief    CMSIS-Core(M) Device Startup File for
 *           Device SAMD21E17A
 * @version  V1.0.0
 * @date     20. January 2021
 ******************************************************************************/
/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Modifications:
 *  - 2025-11-09: Added SAMD21 startup sequence support and refactored vector table (Gian Moreira)
 */

#include "startup_samd21e17a.h"

/*---------------------------------------------------------------------------
  External References
 *---------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
extern uint32_t __STACK_SEAL;
#endif

/*---------------------------------------------------------------------------
  Internal References
 *---------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
void Default_Handler(void);

/*---------------------------------------------------------------------------
  Exception / Peripherals Handler
 *---------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak));
void MemManage_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void SecureFault_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

/* Peripherals handlers */
void PM_Handler                ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SYSCTRL_Handler           ( void ) __attribute__ ((weak, alias("Default_Handler")));
void WDT_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));
void EIC_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));
void NVMCTRL_Handler           ( void ) __attribute__ ((weak, alias("Default_Handler")));
void DMAC_Handler              ( void ) __attribute__ ((weak, alias("Default_Handler")));
void USB_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));
void EVSYS_Handler             ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SERCOM0_Handler           ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SERCOM1_Handler           ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SERCOM2_Handler           ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SERCOM3_Handler           ( void ) __attribute__ ((weak, alias("Default_Handler")));
void Reserved13                ( void ) __attribute__ ((weak, alias("Default_Handler")));
void Reserved14                ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TCC0_Handler              ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TCC1_Handler              ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TCC2_Handler              ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TC3_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TC4_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TC5_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));
void Reserved21                ( void ) __attribute__ ((weak, alias("Default_Handler")));
void Reserved22                ( void ) __attribute__ ((weak, alias("Default_Handler")));
void ADC_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));
void AC_Handler                ( void ) __attribute__ ((weak, alias("Default_Handler")));
void DAC_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));
void PTC_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));
void I2S_Handler               ( void ) __attribute__ ((weak, alias("Default_Handler")));


/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

typedef void(*VECTOR_TABLE_Type)(void);

const VECTOR_TABLE_Type __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP),  /*     Initial Stack Pointer */
  Reset_Handler,                       /*     Reset Handler */
  NMI_Handler,                         /* -14 NMI Handler */
  HardFault_Handler,                   /* -13 Hard Fault Handler */
  MemManage_Handler,                   /* -12 MPU Fault Handler */
  BusFault_Handler,                    /* -11 Bus Fault Handler */
  UsageFault_Handler,                  /* -10 Usage Fault Handler */
  SecureFault_Handler,                 /*  -9 Secure Fault Handler */
  0,                                   /*     Reserved */
  0,                                   /*     Reserved */
  0,                                   /*     Reserved */
  SVC_Handler,                         /*  -5 SVCall Handler */
  DebugMon_Handler,                    /*  -4 Debug Monitor Handler */
  0,                                   /*     Reserved */
  PendSV_Handler,                      /*  -2 PendSV Handler */
  SysTick_Handler,                     /*  -1 SysTick Handler */

  /* Interrupts */
  PM_Handler, 
  SYSCTRL_Handler, 
  WDT_Handler, 
  RTC_Handler, 
  EIC_Handler, 
  NVMCTRL_Handler, 
  DMAC_Handler, 
  USB_Handler, 
  EVSYS_Handler, 
  SERCOM0_Handler, 
  SERCOM1_Handler, 
  SERCOM2_Handler, 
  SERCOM3_Handler, 
  0,                                /*     Reserved */
  0,                                /*     Reserved */
  TCC0_Handler, 
  TCC1_Handler, 
  TCC2_Handler, 
  TC3_Handler, 
  TC4_Handler, 
  TC5_Handler, 
  0,                                /*     Reserved */
  0,                                /*     Reserved */
  ADC_Handler, 
  AC_Handler, 
  DAC_Handler, 
  PTC_Handler, 
  I2S_Handler  
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*---------------------------------------------------------------------------
  Reset Handler called on controller reset
 *---------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void)
{
  __set_PSP((uint32_t)(&__INITIAL_SP));

  SystemInit();                     /* CMSIS System Initialization */
  main();                           /* Enter main */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*---------------------------------------------------------------------------
  Hard Fault Handler
 *---------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
  while(1);
}

/*---------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *---------------------------------------------------------------------------*/
void Default_Handler(void)
{
  while(1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif
