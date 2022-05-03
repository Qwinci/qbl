#pragma once
#include <stdint.h>

using BOOLEAN = bool;
using INTN = intptr_t;
using UINTN = uintptr_t;
using INT8 = int8_t;
using UINT8 = uint8_t;
using INT16 = int16_t;
using UINT16 = uint16_t;
using INT32 = int32_t;
using UINT32 = uint32_t;
using INT64 = int64_t;
using UINT64 = uint64_t;
using CHAR8 = char;
using CHAR16 = wchar_t;
using VOID = void;
struct EFI_GUID {
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t Data4[8];
};
struct EFI_TIME {
	UINT8 Day;
	UINT8 Daylight;
	UINT8 Hour;
	UINT8 Minute;
	UINT8 Month;
	UINT32 Nanosecond;
	UINT8 Pad1;
	UINT8 Pad2;
	UINT8 Second;
	INT16 TimeZone;
	UINT16 Year;
};

#define ENCODE_ERROR(StatusCode)  ((UINTN)(1ULL << 61 | (StatusCode)))
#define ENCODE_WARNING(StatusCode)  ((UINTN)(StatusCode))
enum class EfiStatus : UINTN {
	Success = 0,
	LoadError = ENCODE_ERROR(1),
	InvalidParameter = ENCODE_ERROR(2),
	Unsupported = ENCODE_ERROR(3),
	BadBufferSize = ENCODE_ERROR(4),
	BufferTooSmall = ENCODE_ERROR(5),
	NotReady = ENCODE_ERROR(6),
	DeviceError = ENCODE_ERROR(7),
	WriteProtected = ENCODE_ERROR(8),
	OutOfResources = ENCODE_ERROR(9),
	VolumeCorrupted = ENCODE_ERROR(10),
	VolumeFull = ENCODE_ERROR(11),
	NoMedia = ENCODE_ERROR(12),
	MediaChanged = ENCODE_ERROR(13),
	NotFound = ENCODE_ERROR(14),
	AccessDenied = ENCODE_ERROR(15),
	NoResponse = ENCODE_ERROR(16),
	NoMapping = ENCODE_ERROR(17),
	Timeout = ENCODE_ERROR(18),
	NotStarted = ENCODE_ERROR(19),
	AlreadyStarted = ENCODE_ERROR(20),
	Aborted = ENCODE_ERROR(21),
	ICMPError = ENCODE_ERROR(22),
	TFTPError = ENCODE_ERROR(23),
	ProtocolError = ENCODE_ERROR(24),
	IncompatibleVersion = ENCODE_ERROR(25),
	SecurityViolation = ENCODE_ERROR(26),
	CrcError = ENCODE_ERROR(27),
	EndOfFile = ENCODE_ERROR(31),
	InvalidLanguage = ENCODE_ERROR(32),
	CompromisedData = ENCODE_ERROR(33),
	HTTPError = ENCODE_ERROR(34),
	WarnUnknownGlyph = ENCODE_WARNING(1),
	WarnDeleteFailure = ENCODE_WARNING(2),
	WarnWriteFailure = ENCODE_WARNING(3),
	WarnBufferTooSmall = ENCODE_WARNING(4),
	WarnStaleData = ENCODE_WARNING(5),
	WarnFileSystem = ENCODE_WARNING(6)
};

#define EFI_ERROR(Status) ((INTN)(UINTN)(Status) < 0)

using EFI_HANDLE = VOID*;
using EFI_EVENT = VOID*;
using EFI_PHYSICAL_ADDRESS = UINTN;
using EFI_VIRTUAL_ADDRESS = UINTN;

#define IN
#define OUT
#define OPTIONAL
#define EFIAPI __cdecl

struct EFI_TABLE_HEADER {
	UINT64 Signature;
	UINT32 Revision;
	UINT32 HeaderSize;
	UINT32 CRC32;
	UINT32 Reserved;
};

enum class EfiAllocateType {
	AllocateAnyPages,
	AllocateMaxAddress,
	AllocateAddress,
	MaxAllocateType
};

struct EFI_MEMORY_DESCRIPTOR {
	UINT32 Type;
	EFI_PHYSICAL_ADDRESS PhysicalAddress;
	EFI_VIRTUAL_ADDRESS VirtualAddress;
	UINT64 NumberOfPages;
	UINT64 Attribute;
};

enum class EfiMemoryType {
	ReservedMemoryType,
	LoaderCode,
	LoaderData,
	BootServicesCode,
	BootServicesData,
	RuntimeServicesCode,
	RuntimeServicesData,
	ConventionalMemory,
	UnusableMemory,
	ACPIReclaimMemory,
	ACPIMemoryNVS,
	MemoryMappedIO,
	MemoryMappedIOPortSpace,
	PalCode,
	PersistentMemory,
	MaxMemoryType
};

struct EFI_BOOT_SERVICES {
	EFIAPI EfiStatus AllocatePages(EfiAllocateType Type, EfiMemoryType MemoryType, UINTN Pages, EFI_PHYSICAL_ADDRESS* Memory) {
		return AllocatePages_(Type, MemoryType, Pages, Memory);
	}
	EFIAPI EfiStatus FreePages(EFI_PHYSICAL_ADDRESS* Memory, UINTN Pages) {
		return FreePages_(Memory, Pages);
	}
	EFIAPI EfiStatus GetMemoryMap(UINTN* MapSize, EFI_MEMORY_DESCRIPTOR* Map, UINTN* MapKey, UINTN* DescriptorSize,
								  UINT32* DescriptorVersion) {
		return GetMemoryMap_(MapSize, Map, MapKey, DescriptorSize, DescriptorVersion);
	}
	EFIAPI EfiStatus AllocatePool(EfiMemoryType Type, UINTN Size, VOID** Buffer) {
		return AllocatePool_(Type, Size, Buffer);
	}
	EFIAPI EfiStatus FreePool(VOID* Buffer) {
		return FreePool_(Buffer);
	}
	EFIAPI EfiStatus HandleProtocol(EFI_HANDLE Handle, EFI_GUID* Protocol, VOID** Interface) {
		return HandleProtocol_(Handle, Protocol, Interface);
	}
	EFIAPI EfiStatus ExitBootServices(EFI_HANDLE ImageHandle, UINTN MapKey) {
		return ExitBootServices_(ImageHandle, MapKey);
	}
	EFIAPI EfiStatus LocateProtocol(EFI_GUID* Protocol, VOID* Registration, VOID** Interface) {
		return LocateProtocol_(Protocol, Registration, Interface);
	}
private:
	EFI_TABLE_HEADER Hdr;

	uintptr_t RaiseTPL;
	uintptr_t RestoreTPL;

	EfiStatus (EFIAPI* AllocatePages_)(IN EfiAllocateType Type,
			IN EfiMemoryType MemoryType,
			IN UINTN Pages,
			IN OUT EFI_PHYSICAL_ADDRESS* Memory);
	EfiStatus (EFIAPI* FreePages_)(IN EFI_PHYSICAL_ADDRESS* Memory,
			IN UINTN Pages);
	EfiStatus (EFIAPI* GetMemoryMap_)(
			IN OUT UINTN* MemoryMapSize,
			OUT EFI_MEMORY_DESCRIPTOR* MemoryMap,
			OUT UINTN* MapKey,
			OUT UINTN* DescriptorSize,
			OUT UINT32* DescriptorVersion
			);
	EfiStatus (EFIAPI* AllocatePool_)(
			IN EfiMemoryType PoolType,
			IN UINTN Size,
			OUT VOID** Buffer
			);
	EfiStatus (EFIAPI* FreePool_)(IN VOID* Buffer);

	uintptr_t CreateEvent;
	uintptr_t SetTimer;
	uintptr_t WaitForEvent;
	uintptr_t SignalEvent;
	uintptr_t CloseEvent;
	uintptr_t CheckEvent;

	uintptr_t InstallProtocolInterface;
	uintptr_t ReinstallProtocolInterface;
	uintptr_t UninstallProtocolInterface;
	EfiStatus (EFIAPI* HandleProtocol_)(
			IN EFI_HANDLE Handle,
			IN EFI_GUID* Protocol,
			OUT VOID** Interface
			);
	VOID* Reserved;
	uintptr_t RegisterProtocolNotify;
	uintptr_t LocateHandle;
	uintptr_t LocateDevicePath;
	uintptr_t InstallConfigurationTable;

	uintptr_t LoadImage;
	uintptr_t StartImage;
	uintptr_t Exit;
	uintptr_t UnloadImage;
	EfiStatus (EFIAPI* ExitBootServices_)(
			IN EFI_HANDLE ImageHandle,
			IN UINTN MapKey
			);

	uintptr_t GetNextMonotonicCount;
	EfiStatus (EFIAPI* Stall_)(IN UINTN Microseconds);
	uintptr_t SetWatchdogTimer;

	uintptr_t ConnectController;
	uintptr_t DisconnectController;

	uintptr_t OpenProtocol;
	uintptr_t CloseProtocol;
	uintptr_t OpenProtocolInformation;

	uintptr_t ProtocolsPerHandle;
	uintptr_t LocateHandleBuffer;
	EfiStatus (EFIAPI* LocateProtocol_)(
			IN EFI_GUID* Protocol,
			OPTIONAL IN VOID* Registration,
			OUT VOID** Interface
			);
	uintptr_t InstallMultipleProtocolInterfaces;
	uintptr_t UninstallMultipleProtocolInterfaces;

	uintptr_t CalculateCrc32;

	uintptr_t CopyMem;
	uintptr_t SetMem;
	uintptr_t CreateEventEx;
};

struct EFI_RUNTIME_SERVICES {
	EFI_TABLE_HEADER Hdr;

	uintptr_t GetTime;
	uintptr_t SetTime;
	uintptr_t GetWakeupTime;
	uintptr_t SetWakeupTime;

	uintptr_t SetVirtualAddressMap;
	uintptr_t ConvertPointer;

	uintptr_t GetVariable;
	uintptr_t GetNextVariableName;
	uintptr_t SetVariable;

	uintptr_t GetNextHighMonotonicCount;
	uintptr_t ResetSystem;

	uintptr_t UpdateCapsule;
	uintptr_t QueryCapsuleCapabilities;

	uintptr_t QueryVariableInfo;
};

struct EFI_CONFIGURATION_TABLE {
	EFI_GUID VendorGuid;
	VOID* VendorTable;
};

struct EFI_INPUT_KEY {
	UINT16 ScanCode;
	UINT16 UnicodeChar;
};

struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
	EFIAPI EfiStatus Reset(BOOLEAN ExtendedVerification) {
		return Reset_(this, ExtendedVerification);
	}
	EFIAPI EfiStatus ReadKeyStroke(EFI_INPUT_KEY* Key) {
		return ReadKeyStroke_(this, Key);
	}
private:
	EfiStatus (EFIAPI* Reset_)(IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL* This,
			IN BOOLEAN ExtendedVerification);
	EfiStatus (EFIAPI* ReadKeyStroke_)(IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL* This,
			OUT EFI_INPUT_KEY* Key);
	EFI_EVENT WaitForKey;
};

struct EFI_SIMPLE_TEXT_OUTPUT_MODE {
	INT32 MaxMode;
	INT32 Mode;
	INT32 Attribute;
	INT32 CursorColumn;
	INT32 CursorRow;
	BOOLEAN CursorVisible;
};

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
	EFIAPI EfiStatus Reset(BOOLEAN ExtendedVerification) {
		return Reset_(this, ExtendedVerification);
	}
	EFIAPI EfiStatus OutputString(const CHAR16* String) {
		return OutputString_(this, String);
	}
	EFIAPI EfiStatus TestString(const CHAR16* String) {
		return TestString_(this, String);
	}
	EFIAPI EfiStatus QueryMode(UINTN ModeNumber, UINTN* Columns, UINTN* Rows) {
		return QueryMode_(this, ModeNumber, Columns, Rows);
	}
	EFIAPI EfiStatus SetMode(UINTN ModeNumber) {
		return SetMode_(this, ModeNumber);
	}
	EFIAPI EfiStatus SetAttribute(UINTN Attribute) {
		return SetAttribute_(this, Attribute);
	}
	EFIAPI EfiStatus ClearScreen() {
		return ClearScreen_(this);
	}
	EFIAPI EfiStatus SetCursorPosition(UINTN Column, UINTN Row) {
		return SetCursorPosition_(this, Column, Row);
	}
	EFIAPI EfiStatus EnableCursor(BOOLEAN Visible) {
		return EnableCursor_(this, Visible);
	}
private:
	EfiStatus (EFIAPI* Reset_)(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
			IN BOOLEAN ExtendedVerification);

	EfiStatus (EFIAPI* OutputString_)(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
			IN const CHAR16* String);
	EfiStatus (EFIAPI* TestString_)(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
	                                  IN const CHAR16* String);

	EfiStatus (EFIAPI* QueryMode_)(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
			IN UINTN ModeNumber,
			OUT UINTN* Columns,
			OUT UINTN* Rows);
	EfiStatus (EFIAPI* SetMode_)(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
			IN UINTN ModeNumber);
	EfiStatus (EFIAPI* SetAttribute_)(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
			IN UINTN Attribute);

	EfiStatus (EFIAPI* ClearScreen_)(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This);
	EfiStatus (EFIAPI* SetCursorPosition_)(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
			IN UINTN Column,
			IN UINTN Row);
	EfiStatus (EFIAPI* EnableCursor_)(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
			IN BOOLEAN Visible);

	EFI_SIMPLE_TEXT_OUTPUT_MODE     *Mode;
};

struct EFI_SYSTEM_TABLE {
	EFI_TABLE_HEADER Hdr;
	CHAR16* FirmwareVendor;
	UINT32 FirmwareRevision;
	EFI_HANDLE ConsoleInHandle;
	EFI_SIMPLE_TEXT_INPUT_PROTOCOL* ConIn;
	EFI_HANDLE ConsoleOutHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;
	EFI_HANDLE StandardErrorHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* StdErr;
	EFI_RUNTIME_SERVICES* RuntimeServices;
	EFI_BOOT_SERVICES* BootServices;
	UINTN NumberOfTableEntries;
	EFI_CONFIGURATION_TABLE* ConfigurationTable;
};