#include "cpu_a9.h"

#ifdef __cplusplus
extern "C" {
#endif

// https://github.com/qemu/qemu/blob/4633c1e2c576fbabfe5c8c93f4b842504b69c096/hw/arm/xilinx_zynq.c#L44
const uint32_t g_refclock = 33333333u; // 33.3 MHz

#ifdef __cplusplus
}
#endif
