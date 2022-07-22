#pragma once
#include "types.hpp"
#include "stip.hpp"
#include "stop.hpp"

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

enum class EfiLocateSearchType {
	AllHandles,
	ByRegisterNotify,
	ByProtocol
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
	inline EFIAPI EfiStatus AllocatePages(EfiAllocateType Type, EfiMemoryType MemoryType, UINTN Pages, EFI_PHYSICAL_ADDRESS* Memory) {
		return AllocatePages_(Type, MemoryType, Pages, Memory);
	}
	inline EFIAPI EfiStatus FreePages(EFI_PHYSICAL_ADDRESS* Memory, UINTN Pages) {
		return FreePages_(Memory, Pages);
	}
	inline EFIAPI EfiStatus GetMemoryMap(UINTN* MapSize, EFI_MEMORY_DESCRIPTOR* Map, UINTN* MapKey, UINTN* DescriptorSize,
								  UINT32* DescriptorVersion) {
		return GetMemoryMap_(MapSize, Map, MapKey, DescriptorSize, DescriptorVersion);
	}
	inline EFIAPI EfiStatus AllocatePool(EfiMemoryType Type, UINTN Size, VOID** Buffer) {
		return AllocatePool_(Type, Size, Buffer);
	}
	inline EFIAPI EfiStatus FreePool(VOID* Buffer) {
		return FreePool_(Buffer);
	}
	inline EFIAPI EfiStatus HandleProtocol(EFI_HANDLE Handle, EFI_GUID* Protocol, VOID** Interface) {
		return HandleProtocol_(Handle, Protocol, Interface);
	}
	inline EFIAPI EfiStatus ExitBootServices(EFI_HANDLE ImageHandle, UINTN MapKey) {
		return ExitBootServices_(ImageHandle, MapKey);
	}
	inline EFIAPI EfiStatus LocateProtocol(EFI_GUID* Protocol, VOID* Registration, VOID** Interface) {
		return LocateProtocol_(Protocol, Registration, Interface);
	}
	inline EFIAPI EfiStatus LocateHandle(
			EfiLocateSearchType SearchType,
			EFI_GUID* Protocol,
			VOID* SearchKey,
			UINTN* BufferSize,
			EFI_HANDLE* Buffer
	) {
		return LocateHandle_(SearchType, Protocol, SearchKey, BufferSize, Buffer);
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
	EfiStatus (EFIAPI* LocateHandle_)(
			IN EfiLocateSearchType SearchType,
			OPTIONAL IN EFI_GUID* Protocol,
			OPTIONAL IN VOID* SearchKey,
			IN OUT UINTN* BufferSize,
			OUT EFI_HANDLE* Buffer
			);
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