#ifndef COMMON_H
#define COMMON_H

#ifdef USING_SAMD21E17A_MCU
#include <samd21e17a.h>
#include <system_samd21e17a.h>
#endif

#include <stdbool.h>
#include <stdint.h>

#define BIT(n)      (1<<(n))

#endif
