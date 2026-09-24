SECTIONS
{
  .text : { *(.text) }

  . = align(8)

  .bss : { 
    . = __bss_start
    *(.bss) 
    *(.bss.*)
    . = __bss_ned
  } > RAM

  .rodata: {
    *(.rodata) 
    *(.rodata.*)
  }

  .data: {
    *(.data)
    *(.data.*)
  }

  .stack: {
    *(.stack)
  }
}
