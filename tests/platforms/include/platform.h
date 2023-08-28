#ifndef PLATFORM_H
#define PLATFORM_H

#include "error.h"
#include "system_time.h"

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

error_code_t platform_setup_timer(const uint32_t interval_ms);
void platform_enable_timer(void);

void set_tick_limit_before_exit(const int32_t limit);

#ifdef __cplusplus
}
#endif

#endif
