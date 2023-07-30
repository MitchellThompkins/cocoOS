#include "cpu_a9.h"
#include "error.h"
#include "gic.h"
#include "irq.h"
#include "platform.h"
#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif

static void tick(void)
{
    tick_system_time();
}

error_code_t platform_setup_timer(const uint16_t interval_ms)
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

#ifdef __cplusplus
}
#endif
