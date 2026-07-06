#include "timer.h"

#define SYST_CSR ((volatile uint32_t*)0xE000E010u)
#define SYST_RVR ((volatile uint32_t*)0xE000E014u)
#define SYST_CVR ((volatile uint32_t*)0xE000E018u)

#define SYST_CSR_ENABLE    (1u << 0)
#define SYST_CSR_TICKINT   (1u << 1)
#define SYST_CSR_CLKSOURCE (1u << 2)  /* 1 = processor clock */

#define SYST_RVR_MAX (0x00FFFFFFu)

#ifdef __cplusplus
extern "C" {
#endif

error_code_t init_systick(uint32_t interval_ms)
{
    const uint32_t reload = (g_refclock / 1000u * interval_ms) - 1u;

    if(reload > SYST_RVR_MAX)
        return CONFIG_ERR;

    write_reg32(SYST_RVR, reload);
    write_reg32(SYST_CVR, 0u);
    /* processor clock source + interrupt enable; counter not started yet */
    write_reg32(SYST_CSR, SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT);

    return NO_ERR;
}

void toggle_systick(const int enable)
{
    uint32_t csr = read_reg32(SYST_CSR);

    if(enable)
        csr |= SYST_CSR_ENABLE;
    else
        csr &= ~SYST_CSR_ENABLE;

    write_reg32(SYST_CSR, csr);
}

#ifdef __cplusplus
}
#endif
