cmake_minimum_required(VERSION 3.20)

# The kernel is freestanding, so compiler checks must not try to link a Linux
# executable against the target's C runtime.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

project(OSC
    VERSION 0.1.0
    LANGUAGES C ASM
)

# Configure with an AArch64 cross compiler, for example:
# cmake -S . -B build \
#   -DCMAKE_SYSTEM_NAME=Generic \
#   -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
#   -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
#   -DCMAKE_ASM_COMPILER=aarch64-linux-gnu-gcc

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(OSC_SOURCES
    bootloader/low_level_initialization.S
    bootloader/loader.c
)

file (GLOB KERNEL_SOURCES CONFIGURE_DEPENDS kernel/*.c kernel/shell/*.c)
file (GLOB LIB_SOURCES CONFIGURE_DEPENDS lib/*.c)

add_executable(osc_kernel ${OSC_SOURCES} ${KERNEL_SOURCES} ${LIB_SOURCES})

set_target_properties(osc_kernel PROPERTIES
    OUTPUT_NAME "kernel8"
    SUFFIX ".elf"
)

target_include_directories(osc_kernel PRIVATE ./ kernel/)

target_compile_options(osc_kernel PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
    $<$<COMPILE_LANGUAGE:C>:-fno-builtin>
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
)

target_link_options(osc_kernel PRIVATE
    -nostdlib
    -nostartfiles
    "LINKER:--build-id=none"
    "LINKER:-Map=${CMAKE_CURRENT_BINARY_DIR}/osc.map"
    "LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)

set_property(TARGET osc_kernel APPEND PROPERTY LINK_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)

# Produce a raw image alongside the ELF when an objcopy tool is available.
find_program(OSC_OBJCOPY
    NAMES aarch64-linux-gnu-objcopy llvm-objcopy objcopy
)

if(OSC_OBJCOPY)
    add_custom_command(TARGET osc_kernel POST_BUILD
        COMMAND "${OSC_OBJCOPY}" -O binary
            "$<TARGET_FILE:osc_kernel>"
            "${CMAKE_CURRENT_BINARY_DIR}/osc.bin"
        BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/osc.bin"
        COMMENT "Creating osc.bin"
        VERBATIM
    )
endif()
