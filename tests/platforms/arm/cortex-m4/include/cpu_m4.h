#ifndef CPU_M4_H
#define CPU_M4_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _asm(x) asm volatile (x)

extern const uint32_t g_refclock;

static inline void write_reg32(
        uint32_t volatile* const reg,
        uint32_t const value)
{
    *reg = value;
}

static inline uint32_t read_reg32(
        uint32_t volatile* const reg)
{
    return *reg;
}

static inline void cpu_enable_interrupts(void)
{
    _asm("cpsie i");
}

static inline void cpu_disable_interrupts(void)
{
    _asm("cpsid i");
}

#ifdef __cplusplus
}
#endif

#endif
