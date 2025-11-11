/*************************************************************************//**
 * @file     startup_samd21e17a.h
 * @brief    Start-up header files for the SAMD21E17A
 * @version  V1.0.1
 * @date     10 November 2025
 *****************************************************************************/

#ifndef STARTUP_SAMD21E17A_H   /* ToDo: replace '<Device>' with your device name */
#define STARTUP_SAMD21E17A_H

#ifdef __cplusplus
extern "C" {
#endif

#include "samd21e17a.h"
#include "system_samd21e17a.h"

typedef void(*VECTOR_TABLE_Type)(void);
__NO_RETURN void main (void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_<Device>_H */
