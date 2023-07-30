.equ ANGEL_SWI, 0x123456

.global angel_swi_call
.section .text
.align 4
.arm
angel_swi_call:
    stmfd sp!, {lr}
    svc #ANGEL_SWI
    ldmfd sp!, {lr}
    bx lr

