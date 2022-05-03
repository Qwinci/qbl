#pragma once
#include "systemTable.hpp"

#define EFI_LOADED_IMAGE_PROTOCOL_GUID \
{0x5B1B31A1,0x9562,0x11d2,\
{0x8E,0x3F,0x00,0xA0,0xC9,0x69,0x72,0x3B}}

struct EFI_DEVICE_PATH_PROTOCOL {
	UINT8 Type;
	UINT8 SubType;
	UINT8 Length[2];
};

struct EFI_LOADED_IMAGE_PROTOCOL {
	UINT32 Revision;
	EFI_HANDLE ParentHandle;
	EFI_SYSTEM_TABLE* SystemTable;

	EFI_HANDLE DeviceHandle;
	EFI_DEVICE_PATH_PROTOCOL* FilePath;
	VOID* Reserved;

	UINT32 LoadOptionsSize;
	VOID* LoadOptions;

	VOID* ImageBase;
	UINT64 ImageSize;
	EfiMemoryType ImageCodeType;
	EfiMemoryType ImageDataType;
	uintptr_t Unload;
};

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
{0x0964e5b22,0x6459,0x11d2,\
{0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

enum class EfiFileOpenMode : UINT64 {
	Read = 1,
	Write = 2,
	Create = 0x8000000000000000
};
enum class EfiFileAttribute : UINT64 {
	None = 0,
	ReadOnly = 1,
	Hidden = 2,
	System = 4,
	Reserved = 8,
	Directory = 10,
	Archive = 20,
	ValidAttr = 37
};

struct EFI_FILE_PROTOCOL {
	EFIAPI EfiStatus Open(EFI_FILE_PROTOCOL** NewHandle, const CHAR16* FileName,
						  EfiFileOpenMode Mode, EfiFileAttribute attributes = EfiFileAttribute::None) {
		return Open_(this, NewHandle, FileName, Mode, attributes);
	}
	EFIAPI EfiStatus Close() {
		return Close_(this);
	}
	EFIAPI EfiStatus Delete() {
		return Delete_(this);
	}
	EFIAPI EfiStatus Read(UINTN* BufferSize, VOID* Buffer) {
		return Read_(this, BufferSize, Buffer);
	}
	EFIAPI EfiStatus Write(UINTN* BufferSize, VOID* Buffer) {
		return Write_(this, BufferSize, Buffer);
	}
	EFIAPI EfiStatus GetPosition(UINT64* Position) {
		return GetPosition_(this, Position);
	}
	EFIAPI EfiStatus SetPosition(UINT64 Position) {
		return SetPosition_(this, Position);
	}
	EFIAPI EfiStatus GetInfo(EFI_GUID* Type, UINTN* BufferSize, VOID* Buffer) {
		return GetInfo_(this, Type, BufferSize, Buffer);
	}
private:
	UINT64 Revision;
	EfiStatus (EFIAPI* Open_)(
			IN EFI_FILE_PROTOCOL* This,
			OUT EFI_FILE_PROTOCOL** NewHandle,
			IN const CHAR16* FileName,
			IN EfiFileOpenMode OpenMode,
			IN EfiFileAttribute Attributes
			);
	EfiStatus (EFIAPI* Close_)(
			IN EFI_FILE_PROTOCOL* This
			);
	EfiStatus (EFIAPI* Delete_)(
			IN EFI_FILE_PROTOCOL* This
			);
	EfiStatus (EFIAPI* Read_)(
			IN EFI_FILE_PROTOCOL* This,
			IN OUT UINTN* BufferSize,
			IN VOID* Buffer
			);
	EfiStatus (EFIAPI* Write_)(
			IN EFI_FILE_PROTOCOL* This,
			IN OUT UINTN* BufferSize,
			IN VOID* Buffer
			);
	EfiStatus (EFIAPI* GetPosition_)(
			IN EFI_FILE_PROTOCOL* This,
			OUT UINT64* Position
			);
	EfiStatus (EFIAPI* SetPosition_)(
			IN EFI_FILE_PROTOCOL* This,
			IN UINT64 Position
			);
	EfiStatus (EFIAPI* GetInfo_)(
			IN EFI_FILE_PROTOCOL* This,
			IN EFI_GUID* InformationType,
			IN OUT UINTN* BufferSize,
			IN VOID* Buffer
			);
	uintptr_t SetInfo;
	uintptr_t Flush;
	uintptr_t OpenEx;
	uintptr_t ReadEx;
	uintptr_t WriteEx;
	uintptr_t FlushEx;
};

struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
	EFIAPI EfiStatus OpenVolume(EFI_FILE_PROTOCOL** Root) {
		return OpenVolume_(this, Root);
	}
private:
	UINT64 Revision;
	EfiStatus (EFIAPI* OpenVolume_)(IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* This,
			OUT EFI_FILE_PROTOCOL** Root);
};

#define EFI_FILE_INFO_ID \
{0x09576e92,0x6d3f,0x11d2,\
{0x8e, 0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

struct EFI_FILE_INFO {
	UINT64 Size;
	UINT64 FileSize;
	UINT64 PhysicalSize;
	EFI_TIME CreateTime;
	EFI_TIME LastAccessTime;
	EFI_TIME ModificationTime;
	EfiFileAttribute Attribute;
	CHAR16 FileName[];
};

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
{0x9042a9de,0x23dc,0x4a38,\
{0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}

struct EFI_PIXEL_BITMASK {
	UINT32 RedMask;
	UINT32 GreenMask;
	UINT32 BlueMask;
	UINT32 ReservedMask;
};

enum class EfiGraphicsPixelFormat {
	RedGreenBlueReserved8BitPerColor,
	BlueGreenRedReserved8BitPerColor,
	BitMask,
	BltOnly,
	FormatMax
};

struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION {
	UINT32 Version;
	UINT32 HorizontalResolution;
	UINT32 VerticalResolution;
	EfiGraphicsPixelFormat PixelFormat;
	EFI_PIXEL_BITMASK PixelInformation;
	UINT32 PixelsPerScanLine;
};

struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE {
	UINT32 MaxMode;
	UINT32 Mode;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* Info;
	UINTN SizeOfInfo;
	EFI_PHYSICAL_ADDRESS  FrameBufferBase;
	UINTN FrameBufferSize;
};

struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL {
	UINT8 Blue;
	UINT8 Green;
	UINT8 Red;
	UINT8 Reserved;
};

enum class EfiGraphicsOutputBltOperation {
	BltVideoFill,
	BltVideoToBltBuffer,
	BltBufferToVideo,
	BltVideoToVideo,
	BltOperationMax
};

struct EFI_GRAPHICS_OUTPUT_PROTOCOL {
	EFIAPI EfiStatus QueryMode(UINT32 ModeNumber, UINTN* SizeOfInfo,
							   EFI_GRAPHICS_OUTPUT_MODE_INFORMATION** Info) {
		return QueryMode_(this, ModeNumber, SizeOfInfo, Info);
	}
	EFIAPI EfiStatus SetMode(UINT32 ModeNumber) {
		return SetMode_(this, ModeNumber);
	}
	EFIAPI EfiStatus Blt(
			EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BltBuffer,
			EfiGraphicsOutputBltOperation BltOperation,
			UINTN SourceX,
			UINTN SourceY,
			UINTN DestinationX,
			UINTN DestinationY,
			UINTN Width,
			UINTN Height,
			UINTN Delta = 0
			) {
		return Blt_(this, BltBuffer, BltOperation, SourceX, SourceY, DestinationX, DestinationY, Width, Height, Delta);
	}
private:
	EfiStatus (EFIAPI* QueryMode_)(
			IN EFI_GRAPHICS_OUTPUT_PROTOCOL* This,
			IN UINT32 ModeNumber,
			OUT UINTN* SizeOfInfo,
			OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION** Info
			);
	EfiStatus (EFIAPI* SetMode_)(
			IN EFI_GRAPHICS_OUTPUT_PROTOCOL* This,
			IN UINT32 ModeNumber
			);
	EfiStatus (EFIAPI* Blt_)(
			IN EFI_GRAPHICS_OUTPUT_PROTOCOL* This,
			IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BltBuffer,
			OPTIONAL IN EfiGraphicsOutputBltOperation BltOperation,
			IN UINTN SourceX,
			IN UINTN SourceY,
			IN UINTN DestinationX,
			IN UINTN DestinationY,
			IN UINTN Width,
			IN UINTN Height,
			OPTIONAL IN UINTN Delta
			);
public:
	EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* Mode;
};
