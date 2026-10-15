.global _start

_start:
    // Only core 0 performs shared initialization. Other cores must stay out
    // of BSS clearing and UART setup until an SMP startup path is added.
    mrs x0, mpidr_el1
    and x0, x0, #0xffffff
    cbz x0, primary_core

other_core_park:
    wfi
    b other_core_park

primary_core:
    ldr x0, = __bss_start__
    ldr x1, = __bss_end__

clear_bss:
   cmp x0, x1
   b.hs clear_bss_done
   str xzr, [x0], #8
   b clear_bss
clear_bss_done:

set_sp:
    ldr x2, = __stack_top
    mov sp, x2

_bootstrap:
    bl bootstrap_kernel

park:
    wfi
    b park
