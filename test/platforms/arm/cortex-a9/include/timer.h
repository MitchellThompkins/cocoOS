#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <stdint.h>

#include "cpu_a9.h"
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PRIVATE_TIMER_OFFSET (0x600u)
#define GLOBAL_TIMER_OFFSET (0x200u)

#define PTIMER_INTERRUPT_ID (29)
#define GTIMER_INTERRUPT_ID (27)

#define TIMER_ENABLE_BIT_OFFSET (0)
#define IRQ_ENABLE_BIT_OFFSET   (2)
#define PRESCALER_BIT_OFFSET    (8)

#define PRIVATE_AUTO_RELOAD_BIT_OFFSET   (1)
#define GLOBAL_AUTO_INCREMENT_BIT_OFFSET (3)

#define CLEAR_ISR (1)

typedef enum
{
    TIMER_DISABLE = 0,
    TIMER_ENABLE,
} timer_toggle_control_t;

typedef enum
{
    AUTO_DISABLE = 0,
    AUTO_ENABLE,
} timer_auto_control_t;

typedef volatile struct __attribute__((packed, aligned(4))) {
    uint32_t timer_load;             /* 0x0 */
    uint32_t timer_counter;          /* 0x4 */
    uint32_t timer_control;          /* 0x8 */
    uint32_t timer_interrupt_status; /* 0xC */

    /* There are more watchdog registers below
     * here that we aren't using */
} ptimer_registers;

typedef struct __attribute__((packed, aligned(4))) {
    uint32_t lower32;        /* 0x0 */
    uint32_t upper32;        /* 0x4 */
} gtimer_counter_t;

typedef volatile struct __attribute__((packed, aligned(4))) {
    gtimer_counter_t timer_counter;  /* 0x0 */
    uint32_t timer_control;          /* 0x8 */
    uint32_t timer_interrupt_status; /* 0xC */

    /* There are more config registers below
     * here that we aren't using */
} gtimer_registers;

// private timer
error_code_t init_ptimer(
        const timer_auto_control_t continuous,
        const uint16_t clock_period_ms,
        const uint8_t prescaler,
        isr_ptr callback );

void toggle_ptimer(
        const timer_toggle_control_t enable);

const uint32_t get_ptimer_load_val(void);
const uint32_t get_ptimer_counter_val(void);
const bool get_ptimer_interrupt_status(void);

// global timer
error_code_t init_gtimer(
        const timer_auto_control_t continuous,
        const uint16_t clock_period_ms,
        const uint8_t prescaler,
        isr_ptr callback
        );

void toggle_gtimer(
        const timer_toggle_control_t enable);

const uint32_t get_gtimer_load_val(void);
const gtimer_counter_t get_gtimer_counter_val(void);
const bool get_gtimer_interrupt_status(void);

uint64_t get_system_time(void);

#ifdef __cplusplus
}
#endif

#endif
