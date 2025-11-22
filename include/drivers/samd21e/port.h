#ifndef SAMD21E_PORT_H
#define SAMD21E_PORT_H

#include <common.h>
#include <samd21e17a.h>
#include <system_samd21e17a.h>

typedef volatile port_group_registers_t gpio;

extern gpio* const PortA;
extern gpio* const PortIOA;

#endif
