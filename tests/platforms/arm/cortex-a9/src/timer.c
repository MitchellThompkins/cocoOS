#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "timer.h"

#define PT_BASE ((cpu_get_periphbase() + PRIVATE_TIMER_OFFSET))
#define GT_BASE ((cpu_get_periphbase() + GLOBAL_TIMER_OFFSET))

#ifdef __cplusplus
extern "C" {
#endif

/* A timer's period is:
 *
 * (Prescaler + 1) * (Load value + 1)
 * ----------------------------------
 *             Refclock
 */

typedef enum
{
    PRIVATE = 0,
    GLOBAL,
} timer_type_t;

static ptimer_registers *volatile pt_regs;
static gtimer_registers *volatile gt_regs;

static isr_ptr ptimer_isr_callback;
static isr_ptr gtimer_isr_callback;

// static functions
static inline uint32_t const
    get_ptimer_ctrl_settings(void)
{
    return pt_regs->timer_control;
}

static inline uint32_t const
    get_gtimer_ctrl_settings(void)
{
    return gt_regs->timer_control;
}

static void ptimer_isr(void)
{
    write_reg32(
            &(pt_regs->timer_interrupt_status),
            CLEAR_ISR);

    ptimer_isr_callback();
}

static void gtimer_isr(void)
{
    write_reg32(
            &(gt_regs->timer_interrupt_status),
            CLEAR_ISR);

    gtimer_isr_callback();
}

static bool validate_config(
        uint16_t ms,
        uint8_t prescaler,
        timer_type_t timer_type)
{
    const uint64_t max_timer_value =
        (timer_type==PRIVATE) ?
        UINT32_MAX :
        UINT64_MAX ;

    // Max period is the maximum 32-bit value for
    // this prescaler value
    uint64_t max_period
        = (max_timer_value*(prescaler+1)) / g_refclock;

    // Turn that into a milliseconds value
    uint64_t max_ms = max_period * 1000u;

    // milisecs needs to be less than the maximum
    // which can be represented
    return ms >= max_ms;
}

static uint64_t ms_to_timer_value(
        uint16_t ms,
        uint8_t prescaler,
        timer_type_t timer_type)
{

    const double desired_period_seconds
        = ms * 0.001;

    const double scaled_period_seconds =
        desired_period_seconds / (prescaler+1);

    return (uint64_t)(scaled_period_seconds *
                g_refclock) - 1;
}

const uint32_t
    get_ptimer_counter_val(void)
{
    return pt_regs->timer_counter;
}

// private timer controls
error_code_t init_ptimer(
        const timer_auto_control_t continuous,
        const uint16_t clock_period_ms,
        const uint8_t prescaler,
        isr_ptr callback
        )
{

    if( validate_config(
            clock_period_ms, prescaler, PRIVATE ))
    {
        return CONFIG_ERR;
    }

    const uint32_t load_value =
        (uint32_t) ms_to_timer_value(
            clock_period_ms,
            prescaler,
            PRIVATE);

    pt_regs = (ptimer_registers*) PT_BASE;

    uint32_t control_reg = 0;

    toggle_ptimer(TIMER_DISABLE);

    write_reg32( &(pt_regs->timer_load),
                 load_value );

    control_reg |=
        (continuous << PRIVATE_AUTO_RELOAD_BIT_OFFSET) |
        (prescaler << PRESCALER_BIT_OFFSET);

    // Enable IRQ if that's desired
    if(callback != NULL)
    {
        control_reg |=
            (0x1 << IRQ_ENABLE_BIT_OFFSET);

        ptimer_isr_callback = callback;

        irq_register_isr(
            PTIMER_INTERRUPT_ID,
            ptimer_isr);
    }

    write_reg32( &(pt_regs->timer_control),
                 control_reg );

    return NO_ERR;
}

void toggle_ptimer(
        const timer_toggle_control_t enable)
{
    uint32_t control_reg
        = get_ptimer_ctrl_settings();

    control_reg |= (enable << TIMER_ENABLE_BIT_OFFSET);

    write_reg32( &(pt_regs->timer_control),
                 control_reg );
}

// gloabl timer getters
gtimer_counter_t get_gtimer_counter_val(void)
{
    gtimer_counter_t t = {
        gt_regs->timer_counter.lower32,
        gt_regs->timer_counter.upper32,
    };
    return t;
}

// global timer controls
error_code_t init_gtimer(
        const timer_auto_control_t continuous,
        const uint16_t clock_period_ms,
        const uint8_t prescaler,
        isr_ptr callback
        )
{
    if( validate_config(
            clock_period_ms, prescaler, GLOBAL ))
    {
        return CONFIG_ERR;
    }

    const uint64_t load_value = ms_to_timer_value(
            clock_period_ms,
            prescaler,
            GLOBAL);

    gt_regs = (gtimer_registers*) GT_BASE;

    uint32_t control_reg = 0;

    control_reg |=
        (continuous << GLOBAL_AUTO_INCREMENT_BIT_OFFSET) |
        (prescaler << PRESCALER_BIT_OFFSET);

    // Enable IRQ if that's desired
    if(callback != NULL)
    {
        control_reg |=
            (0x1 << IRQ_ENABLE_BIT_OFFSET);

        gtimer_isr_callback = callback;

        irq_register_isr(
            GTIMER_INTERRUPT_ID,
            gtimer_isr);
    }

    toggle_gtimer(TIMER_DISABLE);

    const uint32_t lower32 =
        load_value & 0xFFFFFFFF;

    const uint32_t upper32 =
        (load_value & 0xFFFFFFFF00000000) >> 32;

    write_reg32( &(gt_regs->timer_control),
                 control_reg );

    write_reg32(
            &(gt_regs->timer_counter.lower32),
            lower32 );

    write_reg32(
            &(gt_regs->timer_counter.upper32),
            upper32 );
}

void toggle_gtimer(
        const timer_toggle_control_t enable)
{
    uint32_t control_reg
        = get_gtimer_ctrl_settings();

    control_reg |= (enable << TIMER_ENABLE_BIT_OFFSET);

    write_reg32( &(gt_regs->timer_control),
                 control_reg );
}

#ifdef __cplusplus
}
#endif
