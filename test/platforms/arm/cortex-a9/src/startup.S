/* proc modes */
.equ MODE_FIQ, 0x11
.equ MODE_IRQ, 0x12
.equ MODE_SVC, 0x13

.global _reset
.section .vector_table, "x"
/* prepare to fill fiq stack */
_reset:
    b setup
    b abort_exception /* 0x4  Undefined Instruction */
    b . /* 0x8  Software Interrupt */
    b abort_exception  /* 0xC  Prefetch Abort */
    b abort_exception /* 0x10 Data Abort */
    b . /* 0x14 Reserved */
    b irq_handler /* 0x18 IRQ */
    b . /* 0x1C FIQ */

.section .text
setup:
    /* disable interrupts*/
    cpsid if

    /* set-up the canary value */
    movw r0, #0xABCD
    movt r0, #0xABCD
    ldr r1, =_canary
    str r0, [r1]

    /* set-up to fill fiq and irq stacks */
    movw r0, #0xDEAD
    movt r0, #0xBEEF

    msr cpsr_c, MODE_FIQ
    ldr r1, =__StackFiqBottom
    ldr sp, =__StackFiqTop
    bl fiq_stack_setup

.section .text
/* fill fiq stack and prepare to fill irq stack */
fiq_stack_setup:
    cmp r1, sp
    strlo r0, [r1], #4
    blo fiq_stack_setup

    /* IRQ stack */
    msr cpsr_c, MODE_IRQ
    ldr r1, =__StackIrqBottom
    ldr sp, =__StackIrqTop

    bl irq_stack_setup

.section .text
/* fill irq stack and prepare to fill svc stack */
irq_stack_setup:
    cmp r1, sp
    strlo r0, [r1], #4
    blo irq_stack_setup

    /* Supervisor mode */
    msr cpsr_c, MODE_SVC

    ldr r1, =__StackBottom
    ldr sp, =__StackTop

    bl stack_setup

.section .text
/* fill stack stack and jump to _start*/
stack_setup:
    cmp r1, sp
    strlo r0, [r1], #4
    blo stack_setup

    bl _start

.section .text
abort_exception:
    swi 0xFF
