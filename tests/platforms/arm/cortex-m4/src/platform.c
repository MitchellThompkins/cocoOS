#include "angel.h"
#include "cpu_m4.h"
#include "error.h"
#include "platform.h"
#include "timer.h"

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static int32_t max_ticks = -1;

static void tick(void)
{
    tick_system_time();

    if( max_ticks != -1
        && get_system_time() >= (uint64_t)max_ticks )
    {
        printf("ERR: max_ticks exceeded in a test!\n");
        unexpected_exit();
    }
}

/* SysTick exception handler — referenced by the vector table in startup.S */
void systick_handler(void)
{
    tick();
}

void set_tick_limit_before_exit(const int32_t limit)
{
    max_ticks = limit;
}

void set_expected_exit_tick_limit(const uint32_t limit)
{
    (void)limit;
}

error_code_t platform_setup_timer(const uint32_t interval_ms)
{
    return init_systick(interval_ms);
}

void platform_enable_timer(void)
{
    cpu_enable_interrupts();
    toggle_systick(1);
}

void unexpected_exit(void)
{
    angel_exit((int)RUNTIME_ERR);
}

void expected_exit(void)
{
    angel_exit((int)NO_ERR);
}

#ifdef __cplusplus
}
#endif
