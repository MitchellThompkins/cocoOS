#ifndef CPU_A9_H
#define CPU_A9_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _asm(x) asm volatile (x)

extern const uint32_t g_refclock;

static inline void write_reg32(
        uint32_t volatile* const reg,
        uint32_t const value )
{
    *reg = value;
}

static inline uint32_t read_reg32(
        uint32_t volatile* const reg)
{
    return *reg;
}

static inline uint32_t cpu_get_periphbase(void)
{
    uint32_t result;
    _asm("mrc p15, #4, %0, c15, c0, #0" : "=r" (result));
    return result;
}

static inline void cpu_enable_interrupts(void)
{
    _asm ("cpsie if");
}

static inline void cpu_disable_interrupts(void)
{
    _asm ("cpsid if");
}

typedef enum
{
    NO_ERR = 0,
    CONFIG_ERR,

} error_code_t;

#ifdef __cplusplus
}
#endif

#endif
