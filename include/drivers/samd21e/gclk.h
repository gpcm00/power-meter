#ifndef SAMD21E_GCLK_H
#define SAMD21E_GCLK_H

#include <common.h>
#include <samd21e17a.h>
#include <system_samd21e17a.h>

int setupClk(uint16_t id, uint32_t div, enum LOGIC_LEVEL divsel);

#endif
