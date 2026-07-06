/* Cortex-M4 vector table and reset handler.
 *
 * On Cortex-M, offset 0x0 holds the initial SP value and offset 0x4 holds
 * the reset vector.  All entries are word-sized function pointers (LSB=1 for
 * Thumb), unlike the A-profile branch-instruction style. */

.syntax unified
.thumb

.extern systick_handler

/* ---- vector table ---------------------------------------------------- */
.section .reset, "a"
.word   __StackTop          /* 0x00  initial SP                            */
.word   _reset              /* 0x04  Reset                                 */
.word   default_handler     /* 0x08  NMI                                   */
.word   hardfault_handler   /* 0x0C  HardFault                             */
.word   default_handler     /* 0x10  MemManage                             */
.word   default_handler     /* 0x14  BusFault                              */
.word   default_handler     /* 0x18  UsageFault                            */
.word   0                   /* 0x1C  Reserved                              */
.word   0                   /* 0x20  Reserved                              */
.word   0                   /* 0x24  Reserved                              */
.word   0                   /* 0x28  Reserved                              */
.word   default_handler     /* 0x2C  SVCall                                */
.word   default_handler     /* 0x30  DebugMon                              */
.word   0                   /* 0x34  Reserved                              */
.word   default_handler     /* 0x38  PendSV                                */
.word   systick_handler     /* 0x3C  SysTick                               */

/* ---- handlers --------------------------------------------------------- */
.section .text

.thumb_func
.global _reset
_reset:
    /* set up canary before newlib clears BSS */
    movw r0, #0xABCD
    movt r0, #0xABCD
    ldr  r1, =_canary
    str  r0, [r1]

    bl   _start
    b    .

.thumb_func
hardfault_handler:
    b    .

.thumb_func
default_handler:
    b    .
