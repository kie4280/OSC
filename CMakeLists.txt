cmake_minimum_required(VERSION 3.20)

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

file (GLOB KERNEL_SOURCES kernel/*.c)

add_executable(osc.elf ${OSC_SOURCES} ${KERNEL_SOURCES})

set_target_properties(osc.elf PROPERTIES
    OUTPUT_NAME "osc"
    SUFFIX ".elf"
)

target_include_directories(osc.elf PRIVATE kernel/)

target_compile_options(osc.elf PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
    $<$<COMPILE_LANGUAGE:C>:-fno-builtin>
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
)

target_link_options(osc.elf PRIVATE
    -nostdlib
    -nostartfiles
    "LINKER:--build-id=none"
    "LINKER:-Map=${CMAKE_CURRENT_BINARY_DIR}/osc.map"
    "LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)

set_property(TARGET osc.elf APPEND PROPERTY LINK_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)

# Produce a raw image alongside the ELF when an objcopy tool is available.
find_program(OSC_OBJCOPY
    NAMES aarch64-linux-gnu-objcopy llvm-objcopy objcopy
)

if(OSC_OBJCOPY)
    add_custom_command(TARGET osc.elf POST_BUILD
        COMMAND "${OSC_OBJCOPY}" -O binary
            "$<TARGET_FILE:osc.elf>"
            "${CMAKE_CURRENT_BINARY_DIR}/osc.bin"
        BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/osc.bin"
        COMMENT "Creating osc.bin"
        VERBATIM
    )
endif()
