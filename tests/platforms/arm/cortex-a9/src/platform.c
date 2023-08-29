#include "angel.h"
#include "cpu_a9.h"
#include "error.h"
#include "gic.h"
#include "irq.h"
#include "platform.h"
#include "stdio.h"
#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif

static int32_t max_ticks = -1;

static void tick(void)
{
    tick_system_time();

    if( max_ticks != -1
        && get_system_time() >= max_ticks )
    {
        printf("ERR: max_ticks exceeded in a test!\n");
        unexpected_exit();
    }
}

void set_tick_limit_before_exit(const int32_t limit)
{
    max_ticks = limit;
}

error_code_t platform_setup_timer(const uint32_t interval_ms)
{
    gic_init();
    gic_enable_interrupt(PTIMER_INTERRUPT_ID);
    cpu_enable_interrupts();

    return init_ptimer(AUTO_ENABLE, interval_ms, 0x0, tick);
}

void platform_enable_timer(void)
{
    toggle_ptimer(TIMER_ENABLE);
}

void unexpected_exit(void)
{
    angel_exit( (int)RUNTIME_ERR );
}

void expected_exit(void)
{
    angel_exit( (int)NO_ERR );
}

#ifdef __cplusplus
}
#endif
