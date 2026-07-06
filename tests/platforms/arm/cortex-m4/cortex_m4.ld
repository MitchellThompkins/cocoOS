/* This linker script is a modified
 * arm-none-eabi/lib/ldscripts/armelf.x in order
 * to give us more control of where things get put
 * into the executable by the linker.
 *
 * Used with the mps2-an386 qemu machine */

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
 * |  .reset       |  |  <- vector table + reset handler
 * |  .init        |  |
 * |               |  V
 * |.fini          |
 * |.rodata        |
 * |.ARM.extab     |
 * |.ARM.exidx     |
 * |.eh_frame      |
 * |.preinit_array |  <- const function pointer tables stay in ROM
 * |.init_array    |
 * |.fini_array    |
 * |.ctors         |
 * |.dtors         |
 * |__etext        |  <- LMA of .data begins here
 * |               |
 * |-RAM-----------| 0x20000000
 * |.data          |  <- copied from ROM __etext by newlib _start
 * |.bss           |
 * |.heap          | V
 * |canary         | X
 * |stack_end      | ^
 * |               | |
 * |stack_start    | STACK
 */

SECTIONS
{
    /* Start of ROM section */
    .text RESET_VECTOR :
    {
        . = ALIGN(4);
        KEEP(*(.reset))
        KEEP (*(.init))
        *(.text .stub .text.* .gnu.linkonce.t.*)
    } > ROM

    .fini :
    {
        KEEP (*(SORT_NONE(.fini)))
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

    /* Constructor / destructor pointer tables are read-only; keep in ROM so
     * __libc_init_array() can read them directly without a copy. */
    .preinit_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > ROM

    .init_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > ROM

    .fini_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > ROM

    .ctors :
    {
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
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

    /* __etext marks the ROM address where .data's image begins.
     * newlib's _start copies [__etext, __etext+sizeof(.data)) -> RAM. */
    . = ALIGN(4);
    PROVIDE (__etext = .);
    PROVIDE (_etext = .);
    PROVIDE (etext = .);

    /* Start of RAM section */
    .data : AT(__etext)
    {
        . = ALIGN(4);
        __data_start = .;
        __data_start__ = .;
        *(vtable)
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
        . = ALIGN(4);
        __data_end = .;
        __data_end__ = .;
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

    . = .;
    .heap (COPY):
    {
        . = ALIGN(4);
        __end__ = .;
        PROVIDE(end = .);
        PROVIDE(__heap_end__ = .);
        *(.heap*)
        . += HEAP_LIMIT;
        __HeapLimit = .;
    } > RAM

    . = .;
    _canary = __HeapLimit;
    __canary_end = _canary + CANARY_SIZE;

    . = ALIGN(4);
    __StackBottom = __canary_end;
    _stack = __StackBottom;
    __StackTop = __StackBottom + STACK_LIMIT;
    PROVIDE(__stack = __StackTop);

    ASSERT( SIZEOF(.data) +
            HEAP_LIMIT +
            CANARY_SIZE +
            STACK_LIMIT <= LENGTH(RAM),
            "region RAM overflowed")
}
