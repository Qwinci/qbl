set(CMAKE_SYSTEM_NAME Generic)

set(triple x86_64-unknown-elf)
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

if(NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    message(FATAL_ERROR "Only clang supported")
endif()

add_compile_options(-mno-sse -mno-sse2 -mno-red-zone -fno-exceptions -fno-rtti)
add_link_options(-nostdlib -nostartfiles -fuse-ld=lld -Wl,-entry=kmain -mcmodel=kernel -T ${PROJECT_SOURCE_DIR}/kernel.ld -static -fpie)