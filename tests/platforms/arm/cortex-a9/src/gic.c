#include "cpu_a9.h"
#include "gic.h"

#define GIC_IFACE_BASE ((cpu_get_periphbase() + GIC_IFACE_OFFSET))
#define GIC_DIST_BASE  ((cpu_get_periphbase() + GIC_DISTRIBUTOR_OFFSET))

#ifdef __cplusplus
extern "C" {
#endif

static gic_distributor_registers *volatile gic_dregs;
static gic_cpu_interface_registers *volatile gic_ifregs;

void gic_init(void)
{
    gic_ifregs = (gic_cpu_interface_registers*) GIC_IFACE_BASE;
    gic_dregs = (gic_distributor_registers*) GIC_DIST_BASE;

    // Enable all interrupt priorities
    write_reg32( &(gic_ifregs->CCPMR),
                 0xFFFFu );

    // Enable interrupt forwarding to this CPU
    write_reg32( &(gic_ifregs->CCTLR),
                 CCTRL_ENABLE );

    // Enable the interrupt distributor
    write_reg32( &(gic_dregs->DCTLR),
                 DCTRL_ENABLE );
}

void gic_enable_interrupt(uint16_t number)
{
    /* Enable the interrupt */
    uint8_t reg = number / 32;
    uint8_t bit = number % 32;

    uint32_t reg_val = gic_dregs->DISENABLER[reg];
    reg_val |= (1u << bit);

    write_reg32( &(gic_dregs->DISENABLER[reg]),
                 reg_val );

    /* Forward interrupt to CPU Interface 0 */
    reg = number / 4;
    bit = (number % 4) * 8; /* Can result in bit 0, 8, 16 or 24 */

    reg_val = gic_dregs->DITARGETSR[reg];
    reg_val |= (1u << bit);

    write_reg32( &(gic_dregs->DITARGETSR[reg]),
                 reg_val );
}

uint16_t gic_acknowledge_interrupt(void)
{
    return gic_ifregs->CIAR & CIAR_ID_MASK;
}

void gic_end_interrupt(uint16_t number)
{
    write_reg32( &(gic_ifregs->CEOIR),
                 (number & CEOIR_ID_MASK) );
}

#ifdef __cplusplus
}
#endif
