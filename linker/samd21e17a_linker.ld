/* Linker script for ATSAMD21E17A
 * Flash: 128 KB (0x20000)
 * RAM:   16 KB  (0x4000)
 */

ENTRY(Reset_Handler)

/* Memory layout */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 128K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}

/* Stack top defined at end of RAM */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Sections */
SECTIONS
{
    /* Interrupt vector table */
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    /* Program code and read-only data */
    .text :
    {
        *(.text*)            /* Code */
        *(.rodata*)          /* Read-only data */
        *(.glue_7) *(.glue_7t)
        *(.eh_frame*)

        KEEP(*(.init))
        KEEP(*(.fini))
    } > FLASH

    /* C runtime initialization and finalization arrays */
    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > FLASH

    /* Initialized data copied from FLASH to RAM */
    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > RAM

    /* Uninitialized data (zeroed at startup) */
    .bss (NOLOAD) :
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > RAM

    /* Stack section */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + 0x1000;   /* Optional fixed stack size (4 KB) */
        _estack = .;
    } > RAM

    /* Symbols for startup code */
    __end__ = .;
    PROVIDE(end = .);
}

/* Provide aliases used by startup code */
PROVIDE(__StackTop = _estack);
PROVIDE(__StackLimit = ORIGIN(RAM));
PROVIDE(__data_start__ = __data_start__);
PROVIDE(__data_end__   = __data_end__);
PROVIDE(__bss_start__  = __bss_start__);
PROVIDE(__bss_end__    = __bss_end__);