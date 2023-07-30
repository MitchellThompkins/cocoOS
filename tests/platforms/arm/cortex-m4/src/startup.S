/* proc modes */
.equ MODE_FIQ, 0x11
.equ MODE_IRQ, 0x12
.equ MODE_SVC, 0x13


/* Newlib handles setting up the C-environment so
 * we don't need to do anything special for the
 * startup, but as an exercise to indicate that it
 * is unused we do fill up the custom FIQ and IRQ
 * stacks with data.
 *
 * Currently the IRQ and FIQ interrupts aren't
 * used, but its best to set them up just-in-case.
 *
 * The _reset function is just a wrapper around
 * getting to _start, which lets us place the
 * first instruction at the reset vector of 0x0,
 * or wherever we want, as that's where
 * instructions starts on a typical bare-metal
 * application.
 * */
.global _reset
.section .reset
/* Prepare to fill FIQ stack */
_reset:
    b setup
    b abort_exception /* 0x4  Undefined Instruction */
    b . /* 0x8  Software Interrupt */
    b abort_exception  /* 0xC  Prefetch Abort */
    b abort_exception /* 0x10 Data Abort */
    b . /* 0x14 Reserved */
    b . /* 0x18 IRQ */
    b . /* 0x1C FIQ */

setup:
    /* Set-up the canary value */
    movw r0, #0xABCD
    movt r0, #0xABCD
    ldr r1, =_canary
    str r0, [r1]

    /* Do newlib startup */
    bl _start

abort_exception:
    swi 0xFF
