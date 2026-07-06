#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#include "cpu_m4.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

error_code_t init_systick(uint32_t interval_ms);
void toggle_systick(int enable);

#ifdef __cplusplus
}
#endif

#endif
