#include "system_time.h"

#ifdef __cplusplus
extern "C" {
#endif

static uint64_t system_time = 0;

void tick_system_time(void)
{
    system_time++;
}

uint64_t get_system_time(void)
{
    return system_time;
}

#ifdef __cplusplus
}
#endif
