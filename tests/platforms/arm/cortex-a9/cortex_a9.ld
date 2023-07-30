/* This linker script is a modified
 * arm-none-eabi/lib/ldscripts/armelf.x in order
 * to give us more control of where things get put
 * into the executable by the linker.
 *
 * Used with the xilinx-zynq-a9 qemu machine*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm",
              "elf32-littlearm")

ENTRY(_reset)

RESET_VECTOR = 0x0;

HEAP_LIMIT  = 1M;
STACK_LIMIT = 1M;
STACK_FIQ_LIMIT = 64K;
STACK_IRQ_LIMIT = 64K;

CANARY_SIZE = 0x4;

MEMORY
{
  ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00300000 /*3M*/
  RAM (xrw) : ORIGIN = 0x00300000, LENGTH = 0x00500000 /*5M*/
}

/* |-ROM-----------| 0x0
 * |.text          |  |
 * |               |  |
 * |               |  V
 * |-RAM-----------|
 * |.data          |
 * |  .preinit_arr |
 * |  .init_arr    |
 * |  .fini_arr    |
 * |               |
 * |.bss           |
 * |               |
 * |.heap_end      | HEAP
 * |               | |
 * |               | |
 * |               | V
 * |canary         | X
 * |stack_end      | ^
 * |               | |
 * |               | |
 * |stack_start    | STACK
 * |fiq_stack_end  |
 * |               | ^
 * |               | |
 * |stack_start    | STACK
 * |irq_stack_end  |
 * |               | ^
 * |               | |
 * |stack_start    | STACK
 */

SECTIONS
{
    /* Start of ROM section */
    .text RESET_VECTOR :
    {
        . = ALIGN(4);
        KEEP(*(.vector_table))
        KEEP (*(.init))
        KEEP (*(.ctors))
        KEEP (*(.fini))
        KEEP (*(.dtors))
        *(.text .stub .text.* .gnu.linkonce.t.*)
    } > ROM

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    } > ROM

    .ARM.extab :
    {
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > ROM

    .ARM.exidx :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__exidx_start = .);
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        PROVIDE_HIDDEN (__exidx_end = .);
    } > ROM

    .eh_frame : ONLY_IF_RO
    {
        . = ALIGN(4);
        KEEP (*(.eh_frame)) *(.eh_frame.*)
    } > ROM

    PROVIDE (__etext = .);
    PROVIDE (_etext = .);
    PROVIDE (etext = .);

    /* Start of RAM section */
    .data : AT(__etext)
    {
        . = ALIGN(4);
        __data_start = .;

        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        *(vtable)
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
        __data_end = .;
    } > RAM

    . = .;
    __bss_start = .;
    __bss_start__ = .;
    .bss :
    {
        . = ALIGN(4);
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
    } > RAM
    __bss_end = .;
    _bss_end__ = .;
    __bss_end__ = .;

    /* This section contains data that is not
     * initialized during load,
     * or during the application's initialization
     * sequence.
     */
    . = .;
    .heap (COPY):
    {
        . = ALIGN(4);
        __end__ = .;
        PROVIDE(end = .);
        PROVIDE(__heap_end__ = .);
        *(.heap*)
        . += HEAP_LIMIT ;
        __HeapLimit = .;
    } > RAM

    /* This is a canary where we intend to put a
     * special value to detect heap or stack
     * overflow. */
    . = ALIGN(4);
    . = .;
    _canary = __HeapLimit;
    __canary_end = _canary + CANARY_SIZE;

    /* Set stack top near end of RAM as arm needs
     * a descending stack.
     */
    . = ALIGN(4);
    . = .;
    __StackBottom = __canary_end;
    _stack = __StackBottom;
    __StackTop = __StackBottom + STACK_LIMIT;
    PROVIDE(__stack = __StackTop);

    /* Provide an FIQ and IRQ stack as well */
    . = ALIGN(4);
    . = .;
    __StackFiqBottom = __StackTop;
    __StackFiqTop = __StackFiqBottom + STACK_FIQ_LIMIT;

    . = ALIGN(4);
    . = .;
    __StackIrqBottom = __StackFiqTop;
    __StackIrqTop = __StackIrqBottom + STACK_IRQ_LIMIT;

    /* Check if data + heap + stacks exceeds RAM
     */
    ASSERT( SIZEOF(.data) +
            HEAP_LIMIT +
            CANARY_SIZE +
            STACK_LIMIT +
            STACK_FIQ_LIMIT +
            STACK_IRQ_LIMIT <= LENGTH(RAM),
            "region RAM overflowed")
}

