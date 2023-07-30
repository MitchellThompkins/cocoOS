#ifndef PLATFORM_H
#define PLATFORM_H

#include "error.h"
#include "system_time.h"

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

error_code_t platform_setup_timer(uint16_t interval_ms);
void platform_enable_timer(void);

#ifdef __cplusplus
}
#endif

#endif
