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

void set_expected_exit_tick_limit(const uint32_t limit);

// force exit with code NO_ERR
void expected_exit(void);

// force exit with code RUNTIME_ERR
void unexpected_exit(void);

#ifdef __cplusplus
}
#endif

#endif
