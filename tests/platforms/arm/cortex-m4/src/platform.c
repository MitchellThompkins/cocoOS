#include "platform.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static void tick(void)
{
}

error_code_t platform_setup_timer(const uint16_t interval_ms)
{
}

void platform_enable_timer(void)
{
}

#ifdef __cplusplus
}
#endif
