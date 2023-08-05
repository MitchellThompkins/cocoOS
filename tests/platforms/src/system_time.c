#include "system_time.h"
#include "os_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

static uint64_t system_time = 0;

void tick_system_time(void)
{
    os_tick();
    system_time++;
}

uint64_t get_system_time(void)
{
    return system_time;
}

#ifdef __cplusplus
}
#endif
