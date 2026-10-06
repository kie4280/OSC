.global _start

_start:
    ldr x0, = __bss_start__
    ldr x1, = __bss_end__

clear_bss:
   cmp x0, x1
   b.hs clear_bss_done
   str xzr, [x0], #8
   b clear_bss
clear_bss_done:

set_sp:
    ldr x2, = _stack_top
    mov sp, x2

_bootstrap:
    bl bootstrap_kernel