/* This linker script is a modified
 * arm-none-eabi/lib/ldscripts/armelf.x in order
 * to give us more control of where things get put
 * into the executable by the linker.
 *
 * Used with the olimex-stm32-h405 qemu machine */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm",
              "elf32-littlearm")

ENTRY(_reset)

RESET_VECTOR = 0x0;

HEAP_LIMIT  = 22K;
STACK_LIMIT = 32K;

CANARY_SIZE = 0x4;

MEMORY
{
  ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00100000 /*1024K*/
  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 0x00030000 /*192K*/
}

/* |-ROM-----------| 0x0
 * |.text          |  |
 * |               |  |
 * |               |  V
 * |-RAM-----------|
 * |.preinit_array |
 * |               |
 * |.init_array    |
 * |               |
 * |.fini_array    |
 * |               |
 * |.ctors         |
 * |               |
 * |.dtors         |
 * |               |
 * |.data          |
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
        KEEP(*(.reset))
        KEEP (*(.init))
        KEEP(*(.isr_vector))
        *(.text .stub .text.* .gnu.linkonce.t.*)
    } > ROM

    .fini :
    {
        KEEP (*(SORT_NONE(.fini)))
    } > ROM

    PROVIDE (__etext = .);
    PROVIDE (_etext = .);
    PROVIDE (etext = .);

    .rodata :
    {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    } > ROM

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > ROM

    .ARM.exidx :
    {
        PROVIDE_HIDDEN (__exidx_start = .);
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        PROVIDE_HIDDEN (__exidx_end = .);
    } > ROM

    .eh_frame : ONLY_IF_RO
    {
        KEEP (*(.eh_frame)) *(.eh_frame.*)
    } > ROM

    /* Start of RAM section */
    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > RAM

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > RAM

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > RAM

    .ctors :
    {
      /* gcc uses crtbegin.o to find the start of
         the constructors, so we make sure it is
         first.  Because this is a wildcard, it
         doesn't matter if the user does not
         actually link against crtbegin.o; the
         linker won't look for a file to match a
         wildcard.  The wildcard also means that it
         doesn't matter which directory crtbegin.o
         is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
      /* We don't want to include the .ctor section from
         the crtend.o file until after the sorted ctors.
         The .ctor section from the crtend file contains the
         end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    } > ROM

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    } > ROM

    .data :
    {
        __data_start = .;
        *(vtable)
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
    } > RAM

    . = .;
    __bss_start = .;
    __bss_start__ = .;
    .bss :
    {
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
    } > RAM
    _bss_end__ = .; __bss_end__ = .;

    /* This section contains data that is not
     * initialized during load,
     * or during the application's initialization
     * sequence.
     */
    . = .;
    .heap (COPY):
    {
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
    . = .;
    _canary = __HeapLimit;
    __canary_end = _canary + CANARY_SIZE;

    /* Set stack top near end of RAM as arm needs
     * a descending stack.
     */
    . = .;
    __StackBottom = __canary_end;
    _stack = __StackBottom;
    __StackTop = __StackBottom + STACK_LIMIT;
    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stacks exceeds RAM
     */
    ASSERT( SIZEOF(.data) +
            HEAP_LIMIT +
            STACK_LIMIT <= LENGTH(RAM),
            "region RAM overflowed")
}

