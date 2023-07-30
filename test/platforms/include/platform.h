#ifndef PLATFORM_H
#define PLATFORM_H

#include "system_time.h"

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void platform_setup_timer(void);
void platform_enable_timer(void);

#ifdef __cplusplus
}
#endif

#endif
