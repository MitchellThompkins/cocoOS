#include "cpu_m4.h"

#ifdef __cplusplus
extern "C" {
#endif

// https://github.com/qemu/qemu/blob/master/hw/arm/mps2-tz.c
// mps2-an386 system clock = 25 MHz
const uint32_t g_refclock = 25000000u;

#ifdef __cplusplus
}
#endif
