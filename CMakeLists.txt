cmake_minimum_required(VERSION 3.21)
project(qbl)
include(toolchain.cmake)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS src/*.hpp)

add_executable(qbl.efi ${SRC} ${HEADERS} src/uefi/protocols.hpp)
add_custom_command(TARGET qbl.efi POST_BUILD
        COMMAND mcopy -i fat.img -D o qbl.efi ::/EFI/BOOT/BOOTX64.EFI
        COMMAND mcopy -i fat.img -D o ../qbl.cfg ::
        COMMAND mcopy -i fat.img -D o ../Tamsyn8x16r.psf ::
        COMMAND mcopy -i fat.img -D o ../tests/basicKernel/build/basicKernel ::)

add_custom_target(Run
        COMMAND qemu-system-x86_64 -m 2G -cpu qemu64 -machine q35 -enable-kvm
        -drive if=pflash,file=/usr/share/edk2-ovmf/x64/OVMF_CODE.fd,format=raw,readonly=on
        -drive file=fat.img,format=raw DEPENDS qbl.efi)