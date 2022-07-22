set(CMAKE_SYSTEM_NAME Generic)
enable_language(C CXX ASM_NASM)

set(triple x86_64-unknown-windows)
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_ASM_NASM_OBJECT_FORMAT Win64)

if(NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    message(FATAL_ERROR "Only clang supported")
endif()

add_compile_options(-mno-sse -mno-sse2 -mno-red-zone -fno-exceptions -fno-rtti -fshort-wchar
        -fno-stack-check -fno-stack-protector -mno-stack-arg-probe -msoft-float)
add_link_options(-nostdlib -nostartfiles -fuse-ld=lld -Wl,-entry:efi_main -Wl,-subsystem:efi_application)

find_program(MTOOLS mformat mmd mcopy REQUIRED)
find_program(DD dd REQUIRED)
find_program(CP cp REQUIRED)
find_program(QEMU qemu-system-x86_64 REQUIRED)
if(NOT EXISTS "${PROJECT_BINARY_DIR}/fat.img")
    execute_process(COMMAND dd if=/dev/zero of=fat.img bs=1k count=1440 status=none COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND mformat -i fat.img -f 1440 :: COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND mmd -i fat.img ::/EFI COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND mmd -i fat.img ::/EFI/BOOT COMMAND_ERROR_IS_FATAL ANY)
endif()