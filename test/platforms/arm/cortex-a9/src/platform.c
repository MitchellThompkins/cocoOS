#include "gic.h"
#include "irq.h"
#include "timer.h"
#include "cpu_a9.h"
#include "platform.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static uint16_t interval_ms = 3000;

static void tick(void)
{
    printf("ticking system time arm: %d\n", (int)get_system_time());
    tick_system_time();
}

void platform_setup_timer(void)
{
    gic_init();
    gic_enable_interrupt(PTIMER_INTERRUPT_ID);
    cpu_enable_interrupts();

    init_ptimer(AUTO_ENABLE, interval_ms, 0x0, tick);
}

void platform_enable_timer(void)
{
    toggle_ptimer(TIMER_ENABLE);
}

#ifdef __cplusplus
}
#endif
