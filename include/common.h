#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define BIT(n)          (1<<(n))
#define BIT_MASK(n)     (BIT(n)-1)

enum LOGIC_LEVEL { LOGIC_LOW, LOGIC_HIGH };

#endif
