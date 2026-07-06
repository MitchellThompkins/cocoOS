/* Cortex-M semihosting uses BKPT #0xAB (Thumb-2) instead of SVC #0x123456 */

.syntax unified
.thumb

.global angel_swi_call
.section .text
.thumb_func
angel_swi_call:
    bkpt #0xAB
    bx lr
