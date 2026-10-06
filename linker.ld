MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 256K
    RAM   (rwx) : ORIGIN = 0x80000, LENGTH = 256K
}

SECTIONS
{
    .text :
    {
        *(.text*)
        *(.rodata*)
    } > FLASH

    .data :
    {
        . = ALIGN(8);
        __data_start__ = .;
        *(.data*)
        . = ALIGN(8);
        __data_end__ = .;
    } > RAM AT > FLASH

    .bss (NOLOAD) :
    {
        . = ALIGN(8);
        __bss_start__ = .;
        *(.bss*)
        *(.sbss*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end__ = .;
    } > RAM

    __stack_top = ORIGIN(RAM) + LENGTH(RAM);
    __stack_size = 0x4000;  /* 16 KiB per core */
    __stack_bottom = __stack_top - (4 * __stack_size);

    ASSERT(__bss_end__ <= __stack_bottom,
        "BSS overlaps reserved stacks");
}