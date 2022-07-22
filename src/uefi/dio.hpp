#pragma once
#include "types.hpp"

#define EFI_DISK_IO_PROTOCOL_GUID \
{0xCE345171,0xBA0B,0x11d2,\
{0x8e,0x4F,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

struct EFI_DISK_IO_PROTOCOL {
	UINT64 Revision;
	inline EFIAPI EfiStatus ReadDisk(UINT32 MediaId, UINT64 Offset, UINTN BufferSize, VOID* Buffer) {
		return ReadDisk_(this, MediaId, Offset, BufferSize, Buffer);
	}
	inline EFIAPI EfiStatus WriteDisk(UINT32 MediaId, UINT64 Offset, UINTN BufferSize, VOID* Buffer) {
		return WriteDisk_(this, MediaId, Offset, BufferSize, Buffer);
	}
private:
	EfiStatus (EFIAPI* ReadDisk_)(IN EFI_DISK_IO_PROTOCOL* This,
			IN UINT32 MediaId,
			IN UINT64 Offset,
			IN UINTN BufferSize,
			IN VOID* Buffer);
	EfiStatus (EFIAPI* WriteDisk_)(IN EFI_DISK_IO_PROTOCOL* This,
			IN UINT32 MediaId,
			IN UINT64 Offset,
			IN UINTN BufferSize,
			IN VOID* Buffer);
};