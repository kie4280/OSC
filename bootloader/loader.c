#include "kernel.h"

void bootstrap_kernel() {
    // Initialize hardware components
    init_hardware();

    // Load the operating system kernel
    load_kernel();

    // Transfer control to the kernel
    jump_to_kernel();
}