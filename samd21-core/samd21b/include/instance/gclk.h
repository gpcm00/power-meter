/*
 * Instance header file for ATSAMD21J16B
 *
 * Copyright (c) 2025 Microchip Technology Inc. and its subsidiaries.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* file generated from device description file (ATDF) version 2019-11-25T06:55:40Z */
#ifndef _SAMD21_GCLK_INSTANCE_
#define _SAMD21_GCLK_INSTANCE_


/* ========== Instance Parameter definitions for GCLK peripheral ========== */
#define GCLK_GENDIV_BITS                         (16)
#define GCLK_GEN_NUM                             (9)        /* Number of Generic Clock Generators */
#define GCLK_GEN_NUM_MSB                         (8)        /* Number of Generic Clock Generators - 1 */
#define GCLK_GEN_SOURCE_NUM_MSB                  (8)        /* Number of Generic Clock Sources - 1 */
#define GCLK_INSTANCE_ID                         (3)
#define GCLK_NUM                                 (37)       /* Number of Generic Clock Users */
#define GCLK_SOURCE_DFLL48M                      (7)        /* DFLL48M output */
#define GCLK_SOURCE_DPLL96M                      (8)        /* DPLL96M output */
#define GCLK_SOURCE_GCLKGEN1                     (2)        /* Generic clock generator 1 output */
#define GCLK_SOURCE_GCLKIN                       (1)        /* Generator input pad */
#define GCLK_SOURCE_NUM                          (9)        /* Number of Generic Clock Sources */
#define GCLK_SOURCE_OSC32K                       (4)        /* OSC32K oscillator outpur */
#define GCLK_SOURCE_OSC8M                        (6)        /* OSC8M oscillator output */
#define GCLK_SOURCE_OSCULP32K                    (3)        /* OSCULP32K oscillator output */
#define GCLK_SOURCE_XOSC                         (0)        /* XOSC oscillator output */
#define GCLK_SOURCE_XOSC32K                      (5)        /* XOSC32K oscillator output */

#endif /* _SAMD21_GCLK_INSTANCE_ */
