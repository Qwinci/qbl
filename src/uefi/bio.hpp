#pragma once
#include "types.hpp"

#define EFI_BLOCK_IO_PROTOCOL_GUID \
  { \
    0x964e5b21, 0x6459, 0x11d2, {0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
  }

struct EFI_BLOCK_IO_MEDIA {
	UINT32 MediaId;
	BOOLEAN RemovableMedia;
	BOOLEAN MediaPresent;
	BOOLEAN LogicalPartition;
	BOOLEAN ReadOnly;
	BOOLEAN WriteCaching;
	UINT32 BlockSize;
	UINT32 IoAlign;
	EFI_LBA LastBlock;

	EFI_LBA LowestAlignedBa;
	UINT32 LogicalBlocksPerPhysicalBlock;
	UINT32 OptimalTransferLengthGranularity;
};

struct EFI_BLOCK_IO_PROTOCOL {
	UINT64 Revision;
	EFI_BLOCK_IO_MEDIA* Media;
	inline EFIAPI EfiStatus Reset(BOOLEAN ExtendedVerification) {
		return Reset_(this, ExtendedVerification);
	}
	inline EFIAPI EfiStatus ReadBlocks(UINT32 MediaId, EFI_LBA LBA, UINTN BufferSize, VOID* Buffer) {
		return ReadBlocks_(this, MediaId, LBA, BufferSize, Buffer);
	}
	inline EFIAPI EfiStatus WriteBlocks(UINT32 MediaId, EFI_LBA LBA, UINTN BufferSize, VOID* Buffer) {
		return WriteBlocks_(this, MediaId, LBA, BufferSize, Buffer);
	}
	inline EFIAPI EfiStatus FlushBlocks() {
		return FlushBlocks_(this);
	}
private:
	EfiStatus (EFIAPI* Reset_)(IN EFI_BLOCK_IO_PROTOCOL* This, IN BOOLEAN ExtendedVerification);
	EfiStatus (EFIAPI* ReadBlocks_)(IN EFI_BLOCK_IO_PROTOCOL* This,
			IN UINT32 MediaId,
			IN EFI_LBA LBA,
			IN UINTN BufferSize,
			OUT VOID* Buffer);
	EfiStatus (EFIAPI* WriteBlocks_)(IN EFI_BLOCK_IO_PROTOCOL* This,
			IN UINT32 MediaId,
			IN EFI_LBA LBA,
			IN UINTN BufferSize,
			IN VOID* Buffer);
	EfiStatus (EFIAPI* FlushBlocks_)(IN EFI_BLOCK_IO_PROTOCOL* This);
};