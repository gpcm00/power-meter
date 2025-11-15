/*
 * System configuration file for ATSAMD21E17A
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

 /*
  * Modifications:
  *  - 2025-11-12: Changed header file (gpcm00)
  *  - 2025-11-13: Implemented SystemInit (gpcm00)
  */

#include <startup/startup_samd21e17a.h>
#include <stdint.h>

/**
 * Initial system clock frequency. The System RC Oscillator (RCSYS) provides
 *  the source for the main clock at chip startup.
 */
#define __SYSTEM_CLOCK    (1000000)     // default value at startup

uint32_t SystemCoreClock = __SYSTEM_CLOCK;

enum LOGIC_LEVEL {
    LOW = 0, HIGH,
};

// Initialize XOSC32K --------------------------------------------------------------
__STATIC_INLINE void SetupSysctrlXOSC32K()
{
    // setup n cycles to stabilize and enable xosc32k as external crystal osc mode
    SYSCTRL_REGS->SYSCTRL_XOSC32K = SYSCTRL_XOSC32K_STARTUP_CYCLE4096 |
                                    SYSCTRL_XOSC32K_XTALEN(HIGH);

    // enable clk module once register is setup
    SYSCTRL_REGS->SYSCTRL_XOSC32K |= SYSCTRL_XOSC32K_ENABLE(HIGH);

    while(!(SYSCTRL_REGS->SYSCTRL_XOSC32K & SYSCTRL_PCLKSR_XOSC32KRDY_Msk));
}

// Setup GCLK_GEN1 to output XOSC32K -----------------------------------------------
__STATIC_INLINE void SetupGCLKGEN1()
{
    // set no prescaler value on clk gen 1
    GCLK_REGS->GCLK_GENDIV = GCLK_GENDIV_ID(0x1) |
                             GCLK_GENDIV_DIV(0);

    // enable GCLK_GEN1 with XOSC32K as source and no divider
    GCLK_REGS->GCLK_GENCTRL = GCLK_GENCTRL_ID(0x1) |
                              GCLK_GENCTRL_SRC_XOSC32K |
                              GCLK_GENCTRL_GENEN(HIGH) |
                              GCLK_GENCTRL_DIVSEL(LOW);

    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY(HIGH));

    // route XOSC32K to DFLL48 module
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_ID_DFLL48 |
                              GCLK_CLKCTRL_GEN(0x1) |
                              GCLK_CLKCTRL_CLKEN(HIGH);

    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk);
}

// Setup DFLL48M with XOSC32K --------------------------------------------------
__STATIC_INLINE void SetupDFLL()
{
    // TODO: add coarse and fine callibration from NVM

    // conforming with the device's errata page 9 (1.2.1)
    SYSCTRL_REGS->SYSCTRL_DFLLCTRL &= ~SYSCTRL_DFLLCTRL_ONDEMAND_Msk;
    while (!(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLRDY_Msk));

    SYSCTRL_REGS->SYSCTRL_DFLLCTRL = 0;

    // start dll with maximum coarse and fine values
    SYSCTRL_REGS->SYSCTRL_DFLLMUL = SYSCTRL_DFLLMUL_CSTEP(31) |
                                    SYSCTRL_DFLLMUL_FSTEP(511) |
                                    SYSCTRL_DFLLMUL_MUL(1495);

    SYSCTRL_REGS->SYSCTRL_DFLLCTRL = SYSCTRL_DFLLCTRL_MODE(HIGH) |
                                     SYSCTRL_DFLLCTRL_WAITLOCK(HIGH);

    SYSCTRL_REGS->SYSCTRL_DFLLCTRL |= SYSCTRL_DFLLCTRL_ENABLE_Msk;
    while (!(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLRDY_Msk));
}


void SystemInit(void)
{
    SetupSysctrlXOSC32K();     // enable XOSC32 as crystal oscillator
    SetupGCLKGEN1();           // routes XOSC32 to DFLL48
    SetupDFLL();

    SystemCoreClockUpdate();
    return;
}

void SystemCoreClockUpdate(void)
{
    SystemCoreClock = __SYSTEM_CLOCK;
    return;
}
