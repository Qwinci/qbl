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
using EFI_LBA = UINT64;
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

#undef ENCODE_ERROR
#undef ENCODE_WARNING

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