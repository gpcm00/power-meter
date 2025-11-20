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

#define BIT(n)          (1<<(n))
#define N_BIT_MASK(n)   (BIT(n)-1)

/**
 * Initial system clock frequency. The System RC Oscillator (RCSYS) provides
 *  the source for the main clock at chip startup.
 */
#define __SYSTEM_CLOCK    (1000000)     // default value at startup
#define __GCLK_DEBUG      1             // 1 for debugging

uint32_t SystemCoreClock = __SYSTEM_CLOCK;

enum LOGIC_LEVEL {
    LOW = 0, HIGH,
};

// Initialize XOSC32K -------------------------------------------------------------------
__STATIC_INLINE void SetupSysctrlXOSC32K()
{
    // setup n cycles to stabilize and enable xosc32k as external crystal osc mode
    SYSCTRL_REGS->SYSCTRL_XOSC32K = SYSCTRL_XOSC32K_STARTUP_CYCLE32768 |
                                    SYSCTRL_XOSC32K_EN32K(HIGH) |
                                    SYSCTRL_XOSC32K_XTALEN(HIGH);

    // enable clk module once register is setup
    SYSCTRL_REGS->SYSCTRL_XOSC32K |= SYSCTRL_XOSC32K_ENABLE(HIGH);

    while(!(SYSCTRL_REGS->SYSCTRL_XOSC32K & SYSCTRL_PCLKSR_XOSC32KRDY_Msk));
}

// Setup GCLK_GEN1 to output XOSC32K ----------------------------------------------------
__STATIC_INLINE void SetupGCLKGEN1()
{
    GCLK_REGS->GCLK_CTRL |= GCLK_CTRL_SWRST(HIGH);
    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY(HIGH));

    // set no prescaler value on clk gen 1
    GCLK_REGS->GCLK_GENDIV = GCLK_GENDIV_ID(0x1) |
                             GCLK_GENDIV_DIV(0);

    // enable GCLK_GEN1 with XOSC32K as source and no divider
    GCLK_REGS->GCLK_GENCTRL = GCLK_GENCTRL_ID(0x1) |
                              GCLK_GENCTRL_OE(__GCLK_DEBUG) |
                              GCLK_GENCTRL_SRC_XOSC32K |
                              GCLK_GENCTRL_GENEN(HIGH) |
                              GCLK_GENCTRL_DIVSEL(LOW);

    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY(HIGH));

    // route XOSC32K to DFLL48M module
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_GEN(0x1) |
                              GCLK_CLKCTRL_ID_DFLL48 |
                              GCLK_CLKCTRL_CLKEN(HIGH);

    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk);
}

// Setup DFLL48M with XOSC32K -----------------------------------------------------------
__STATIC_INLINE void SetupDFLL()
{
    // add coarse and fine callibration from NVM
    uint32_t coarse =
        OTP4_FUSES_REGS->FUSES_OTP4_WORD_1 >> FUSES_OTP4_WORD_1_DFLL48M_COARSE_CAL_Pos
                                                                        & N_BIT_MASK(6);

    uint32_t fine =
        OTP4_FUSES_REGS->FUSES_OTP4_WORD_2 >> FUSES_OTP4_WORD_2_DFLL48M_FINE_CAL_Pos
                                                                        & N_BIT_MASK(10);

    // conforming with the device's errata and requesting a clock now
    SYSCTRL_REGS->SYSCTRL_DFLLCTRL = SYSCTRL_DFLLCTRL_ENABLE(HIGH);
    while (!(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLRDY_Msk));

    // update coarse and fine values to speedup startup
    SYSCTRL_REGS->SYSCTRL_DFLLVAL = SYSCTRL_DFLLVAL_COARSE(coarse) |
                                    SYSCTRL_DFLLVAL_FINE(fine);
    while (!(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLRDY_Msk));

    // start dll with maximum coarse and fine step values
    SYSCTRL_REGS->SYSCTRL_DFLLMUL = SYSCTRL_DFLLMUL_CSTEP(31) |
                                    SYSCTRL_DFLLMUL_FSTEP(511) |
                                    SYSCTRL_DFLLMUL_MUL(1495);
    while (!(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLRDY_Msk));

    // start DFLL in close-loop, no quick lock and wait for lock
    SYSCTRL_REGS->SYSCTRL_DFLLCTRL = SYSCTRL_DFLLCTRL_MODE(HIGH) |
                                     SYSCTRL_DFLLCTRL_QLDIS(HIGH) |
                                     SYSCTRL_DFLLCTRL_WAITLOCK(HIGH);
    while (!(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLRDY_Msk));

    SYSCTRL_REGS->SYSCTRL_DFLLCTRL |= SYSCTRL_DFLLCTRL_ENABLE(HIGH);
    while (!(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLLCKC_Msk)
        || !(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLLCKF_Msk));

    while (!(SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLRDY_Msk));

}

// Setup GCLK_GEN0 with DFLL48M ---------------------------------------------------------
__STATIC_INLINE void SetupGCLKGEN0()
{
    /* By default, GLCKGEN0 is the CPU clock */

    // according to NVM characteristics in datasheet, wait one cycle
    NVMCTRL_REGS->NVMCTRL_CTRLB |= NVMCTRL_CTRLB_RWS(1);

    // set no prescaler value on clk gen 0
    GCLK_REGS->GCLK_GENDIV = GCLK_GENDIV_ID(0x0) |
                             GCLK_GENDIV_DIV(0);

    // enable GCLK_GEN0 with DFLL48M as source and no divider
    GCLK_REGS->GCLK_GENCTRL = GCLK_GENCTRL_ID(0x0) |
                              GCLK_GENCTRL_OE(__GCLK_DEBUG) |
                              GCLK_GENCTRL_SRC_DFLL48M |
                              GCLK_GENCTRL_GENEN(HIGH) |
                              GCLK_GENCTRL_DIVSEL(LOW);

    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk);

    // route DFLL48M to USB module
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_GEN(0x0) |
                              GCLK_CLKCTRL_ID_USB |
                              GCLK_CLKCTRL_CLKEN(HIGH);
    while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk);
}

// Disable OSCM8M -----------------------------------------------------------------------
__STATIC_INLINE void SetupOSC8M()
{
    SYSCTRL_REGS->SYSCTRL_OSC8M &= ~SYSCTRL_OSC8M_ENABLE_Msk;
}

void SystemInit(void)
{
    SetupSysctrlXOSC32K();      // enable XOSC32K as crystal oscillator
    SetupGCLKGEN1();            // routes XOSC32K to DFLL48M
    SetupDFLL();                // sets DFLL48M to 48MHz using XOSC32K
    SetupGCLKGEN0();            // uses DFLL48M as the CPU clock
    SetupOSC8M();               // save energy by disabling unused clocks

    SystemCoreClockUpdate();
    return;
}

void SystemCoreClockUpdate(void)
{
    SystemCoreClock = 48000000;
    return;
}
