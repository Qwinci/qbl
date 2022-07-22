#include "uefi/systemTable.hpp"
#include "uefi/protocols.hpp"
#include "elf.hpp"
#include "uefi/bio.hpp"
#include "uefi/dio.hpp"

static void print(EFI_SYSTEM_TABLE* systemTable, uintptr_t number) {
	size_t i = 0;
	for (size_t num = number; num; num /= 10, ++i);
	if (number == 0) {
		systemTable->ConOut->OutputString(L"0\r\n");
		return;
	}
	wchar_t buffer[i + 1];
	buffer[i] = 0;
	while (number) {
		char remainder = static_cast<char>(number % 10);
		buffer[--i] = L'0' + remainder;
		number /= 10;
	}
	systemTable->ConOut->OutputString(buffer);
	systemTable->ConOut->OutputString(L"\r\n");
}

static void hexprint(EFI_SYSTEM_TABLE* systemTable, uintptr_t number) {
	size_t i = 0;
	for (size_t num = number; num; num >>= 4, ++i);
	if (number == 0) {
		systemTable->ConOut->OutputString(L"0\r\n");
		return;
	}
	wchar_t buffer[i + 1];
	buffer[i] = 0;
	while (number) {
		char remainder = static_cast<char>(number & 0xF);
		buffer[--i] = remainder < 10 ? L'0' + remainder : 'A' + remainder - 10;
		number >>= 4;
	}
	systemTable->ConOut->OutputString(L"0x");
	systemTable->ConOut->OutputString(buffer);
	systemTable->ConOut->OutputString(L"\r\n");
}

uint8_t getShift(uint32_t mask) {
	size_t shift = 0;
	while (!(mask & 1)) {
		++shift;
		mask >>= 1;
	}
	return shift;
}

struct Psf2Header {
	uint8_t magic[4];
	uint32_t version;
	uint32_t headerSize;
	uint32_t flags;
	uint32_t length;
	uint32_t charSize;
	uint32_t height;
	uint32_t width;
};

uint32_t getPixel(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop, uint32_t color) {
	uint8_t red = color >> 24;
	uint8_t green = color >> 16 & 0xFF;
	uint8_t blue = color >> 8 & 0xFF;
	uint32_t value;
	if (gop->Mode->Info->PixelFormat == EfiGraphicsPixelFormat::RedGreenBlueReserved8BitPerColor) {
		value = color;
	}
	else if (gop->Mode->Info->PixelFormat == EfiGraphicsPixelFormat::BlueGreenRedReserved8BitPerColor) {
		value = blue | green << 8 | red << 16;
	}
	else {
		value = red << getShift(gop->Mode->Info->PixelInformation.RedMask);
		value |= green << getShift(gop->Mode->Info->PixelInformation.GreenMask);
		value |= blue << getShift(gop->Mode->Info->PixelInformation.BlueMask);
	}
	return value;
}

void setPixel(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop, uint32_t x, uint32_t y, uint32_t color) {
	*((uint32_t*) (gop->Mode->FrameBufferBase + y * gop->Mode->Info->PixelsPerScanLine * 4 + x * 4)) =
			getPixel(gop, color);
}

bool strcmpn(const char* str1, const char* str2, size_t n) {
	for (size_t i = 0; i < n; ++i) if (str1[i] == '\n' || str2[i] == '\n' || str1[i] != str2[i]) return false;
	return true;
}

EFIAPI EfiStatus efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
	SystemTable->ConOut->ClearScreen();
	SystemTable->ConOut->OutputString(L"Hello world!\r\n");

	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	SystemTable->BootServices->LocateProtocol(
			&gopGuid,
			nullptr,
			reinterpret_cast<void**>(&gop)
	);

	if (gop) {
		SystemTable->ConOut->OutputString(L"GOP Located\r\n");
	}

	// --------------------------------------------------------------------------------------

	EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
	EFI_GUID loadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	SystemTable->BootServices->HandleProtocol(
			ImageHandle,
			&loadedImageGuid,
			reinterpret_cast<void**>(&loadedImage)
	);
	if (loadedImage) {
		SystemTable->ConOut->OutputString(L"LoadedImage\r\n");
	}

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fileSystem;
	EFI_GUID fileSystemGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	SystemTable->BootServices->HandleProtocol(
			loadedImage->DeviceHandle,
			&fileSystemGuid,
			reinterpret_cast<void**>(&fileSystem)
	);
	if (fileSystem) {
		SystemTable->ConOut->OutputString(L"FileSystem\r\n");
	}

	EFI_FILE_PROTOCOL* root;
	auto status = fileSystem->OpenVolume(&root);
	if (status == EfiStatus::Success) {
		SystemTable->ConOut->OutputString(L"Root\r\n");
	}

	EFI_FILE_PROTOCOL* fontFile;
	status = root->Open(&fontFile, L"Tamsyn8x16r.psf", EfiFileOpenMode::Read);
	if (status == EfiStatus::Success) {
		SystemTable->ConOut->OutputString(L"font\r\n");
	}

	EFI_GUID fileInfoGuid = EFI_FILE_INFO_ID;
	EFI_FILE_INFO* fileInfo = nullptr;
	UINTN fileInfoSize;
	fontFile->GetInfo(&fileInfoGuid, &fileInfoSize, fileInfo);
	SystemTable->BootServices->AllocatePool(
			EfiMemoryType::LoaderData,
			fileInfoSize,
			reinterpret_cast<void**>(&fileInfo));
	fontFile->GetInfo(&fileInfoGuid, &fileInfoSize, fileInfo);

	void* fontBuffer;
	SystemTable->BootServices->AllocatePool(
			EfiMemoryType::LoaderData,
			fileInfo->FileSize,
			&fontBuffer
			);
	fontFile->Read(&fileInfo->FileSize, fontBuffer);
	fontFile->Close();

	constexpr uint32_t scale = 1;
	auto printString = [gop, fontBuffer](uint32_t col, uint32_t line, const char* string, uint32_t color, uint32_t bgColor) {
		if (!string) return;
		auto* header = reinterpret_cast<Psf2Header*>(fontBuffer);
		col *= header->width;
		line *= header->height;
		for (char character = *string; *string; character = *++string) {
			if (character == '\n') break;
			if (line * scale > gop->Mode->Info->VerticalResolution) {
				return;
			}
			for (uint32_t y = 0; y < header->height * scale; ++y) {
				if (col * scale > gop->Mode->Info->HorizontalResolution) {
					col = 0;
					line += header->height * scale;
				}
				for (uint32_t x = 0; x < header->width * scale; ++x) {
					setPixel(gop, x + col, y + line, *((uint8_t*) header + header->headerSize + character * header->charSize + y / scale) & 1 << (header->width - 1 - x / scale) ? color : bgColor);
				}
			}
			col += header->width * scale;
		}
	};

	EFI_FILE_PROTOCOL* kernelFile;
	status = root->Open(&kernelFile, L"basicKernel", EfiFileOpenMode::Read);
	if (status == EfiStatus::Success) {
		SystemTable->ConOut->OutputString(L"KernelFile\r\n");
	}

	Elf64Hdr hdr; // NOLINT(cppcoreguidelines-pro-type-member-init)
	uint64_t hdrSize = sizeof(Elf64Hdr);
	kernelFile->Read(&hdrSize, &hdr);

	Elf64SHdr strTabHeader; // NOLINT(cppcoreguidelines-pro-type-member-init)
	uint64_t sHdrSize = sizeof(Elf64SHdr);
	kernelFile->SetPosition(hdr.e_shoff + hdr.e_shstrndx * hdr.e_shentsize);
	kernelFile->Read(&sHdrSize, &strTabHeader);

	char* stringTable;
	SystemTable->BootServices->AllocatePool(EfiMemoryType::LoaderData, strTabHeader.sh_size,
	                                        reinterpret_cast<void**>(&stringTable));
	kernelFile->SetPosition(strTabHeader.sh_offset);
	kernelFile->Read(&strTabHeader.sh_size, stringTable);
	size_t line = 3;
	for (size_t i = 0; i < hdr.e_shnum; ++i) {
		kernelFile->SetPosition(hdr.e_shoff + i * hdr.e_shentsize);
		Elf64SHdr sHdr; // NOLINT(cppcoreguidelines-pro-type-member-init)
		sHdrSize = sizeof(Elf64SHdr);
		kernelFile->Read(&sHdrSize, &sHdr);
		printString(0, line++, stringTable + sHdr.sh_name, 0x0000FF00, 0);
	}

	size_t inMemorySize = 0;
	uintptr_t baseAddress = 0;
	bool baseAddressFound = false;
	for (size_t i = hdr.e_phoff; i < hdr.e_phoff + hdr.e_phnum * sizeof(Elf64PHdr); i += sizeof(Elf64PHdr)) {
		Elf64PHdr pHdr; // NOLINT(cppcoreguidelines-pro-type-member-init)
		uint64_t pHdrSize = sizeof(Elf64PHdr);
		kernelFile->SetPosition(i);
		kernelFile->Read(&pHdrSize, &pHdr);
		if (pHdr.p_type == PT_LOAD) {
			if (!baseAddressFound) {
				baseAddress = pHdr.p_vaddr;
				baseAddressFound = true;
			}
			inMemorySize += pHdr.p_memsz;
		}
	}

	uintptr_t kernelMemoryBuffer = 0;
	SystemTable->BootServices->AllocatePages(
			EfiAllocateType::AllocateAnyPages,
			EfiMemoryType::LoaderData,
			(inMemorySize + 0x1000 - 1) / 0x1000, &kernelMemoryBuffer);

	for (size_t i = hdr.e_phoff; i < hdr.e_phoff + hdr.e_phnum * sizeof(Elf64PHdr); i += sizeof(Elf64PHdr)) {
		Elf64PHdr pHdr; // NOLINT(cppcoreguidelines-pro-type-member-init)
		uint64_t pHdrSize = sizeof(Elf64PHdr);
		kernelFile->SetPosition(i);
		kernelFile->Read(&pHdrSize, &pHdr);
		if (pHdr.p_type == PT_LOAD) {
			kernelFile->SetPosition(pHdr.p_offset);
			kernelFile->Read(&pHdr.p_filesz, reinterpret_cast<void*>(kernelMemoryBuffer + pHdr.p_vaddr - baseAddress));
		}
	}

	kernelFile->Close();

	auto main = reinterpret_cast<__attribute__((sysv_abi)) int (*)(void)>(kernelMemoryBuffer + hdr.e_entry - baseAddress);
	auto result = main();
	if (result == 1) {
		SystemTable->ConOut->OutputString(L"kernel works\r\n");
	}

	// --------------------------------------------------------------------------------------

	EFI_FILE_PROTOCOL* configFile;
	status = root->Open(&configFile, L"qbl.cfg", EfiFileOpenMode::Read);
	if (status == EfiStatus::Success) {
		SystemTable->ConOut->OutputString(L"ConfigFile\r\n");
	}

	uint64_t bufferSize = sizeof(EFI_FILE_INFO) + 200 * sizeof(wchar_t);
	SystemTable->BootServices->AllocatePool(EfiMemoryType::LoaderData, bufferSize, reinterpret_cast<void**>(&fileInfo));
	status = configFile->GetInfo(&fileInfoGuid, &bufferSize, fileInfo);
	if (status == EfiStatus::Success) {
		SystemTable->ConOut->OutputString(L"FileInfo\r\n");
	}

	char* charBuffer;
	uint64_t charBufferSize = fileInfo->FileSize;
	SystemTable->BootServices->AllocatePool(EfiMemoryType::LoaderData, charBufferSize,
	                                        reinterpret_cast<void**>(&charBuffer));
	status = configFile->Read(&charBufferSize, charBuffer);
	if (status == EfiStatus::Success) {
		SystemTable->ConOut->OutputString(L"Read\r\n");
	}
	configFile->Close();
	root->Close();
	size_t lineCount = 1;
	for (size_t i = 0; i < charBufferSize; ++i) if (charBuffer[i] == '\n') ++lineCount;
	char* lines[lineCount];
	char* start = charBuffer;
	size_t lineIndex = 0;
	for (size_t i = 0; i < charBufferSize; ++i) {
		if (charBuffer[i] == '\n') lines[lineIndex++] = start, start = charBuffer + i + 1;
	}
	size_t entryCount = 0;
	for (size_t i = 0; i < lineCount; ++i) {
		if (strcmpn(lines[i], "ENTRY=", 6)) {
			++entryCount;
		}
	}

	// --------------------------------------------------------------------------------------

	uint64_t memoryMapSize;
	uint64_t descriptorSize;
	status = SystemTable->BootServices->GetMemoryMap(&memoryMapSize, nullptr, nullptr, &descriptorSize, nullptr);

	EFI_MEMORY_DESCRIPTOR* memoryMap;

	size_t extraSize = sizeof(EFI_MEMORY_DESCRIPTOR);
	while (status == EfiStatus::BufferTooSmall) {
		extraSize += sizeof(EFI_MEMORY_DESCRIPTOR);
		if (memoryMap)
			SystemTable->BootServices->FreePool(memoryMap);
		status = SystemTable->BootServices->AllocatePool(EfiMemoryType::LoaderData, memoryMapSize + extraSize,
															reinterpret_cast<void**>(&memoryMap));
		if (status == EfiStatus::OutOfResources) return EfiStatus::OutOfResources;

		uint64_t mapKey;
		status = SystemTable->BootServices->GetMemoryMap(&memoryMapSize, memoryMap, &mapKey, &descriptorSize, nullptr);
	}

	/*while (status != EfiStatus::Success) {
		uint64_t mapKey;
		SystemTable->BootServices->GetMemoryMap(&memoryMapSize, buffer, &mapKey, &descriptorSize, nullptr);

		status = SystemTable->BootServices->ExitBootServices(ImageHandle, mapKey);
	}*/

	// --------------------------------------------------------------------------------------

	uint64_t selected = 0;
	const char* strings[entryCount];
	const char* bootFileNames[entryCount];

	size_t entry = 0;
	for (size_t i = 0; i < lineCount; ++i) {
		if (strcmpn(lines[i], "ENTRY=", 6)) {
			strings[entry++] = lines[i] + 6;
		}
		else if (strcmpn(lines[i], "KERNEL_PATH=", 12)) {
			const char* string = lines[i] + 12;
			if (strcmpn(string, "boot://", 7)) {
				string += 7;
				if (entry == 0) continue;
				else bootFileNames[entry - 1] = string;
			}
		}
	}
	if (entryCount > 0) {
		printString(0, 0, strings[0], 0xFF000000, 0);
		printString(10, 0, bootFileNames[0], 0xFF000000, 0);
		for (size_t i = 1; i < entryCount; ++i) {
			printString(0, 1, strings[i], 0x00FF0000, 0);
			printString(10, 1, bootFileNames[i], 0x00FF0000, 0);
		}
	}

	SystemTable->ConIn->Reset(false);
	while (true) {
		EFI_INPUT_KEY key{};
		SystemTable->ConIn->ReadKeyStroke(&key);
		if (entryCount > 0) {
			if (key.ScanCode == SCAN_DOWN) {
				if (selected < sizeof(strings) / sizeof(char*) - 1) selected += 1;
				else selected = 0;

				printString(0, selected, strings[selected], 0xFF000000, 0);
				printString(10, selected, bootFileNames[selected], 0xFF000000, 0);
				for (size_t i = 0; i < sizeof(strings) / sizeof(*strings); ++i) {
					if (i != selected) {
						printString(0, i, strings[i], 0x00FF0000, 0);
						printString(10, i, bootFileNames[i], 0x00FF0000, 0);
					}
				}
			}
			else if (key.ScanCode == SCAN_UP) {
				if (selected > 0) {
					selected -= 1;
				}
				else selected = sizeof(strings) / sizeof(*strings) - 1;

				printString(0, selected, strings[selected], 0xFF000000, 0);
				printString(10, selected, bootFileNames[selected], 0xFF000000, 0);
				for (size_t i = 0; i < sizeof(strings) / sizeof(*strings); ++i) {
					if (i != selected) {
						printString(0, i, strings[i], 0x00FF0000, 0);
						printString(10, i, bootFileNames[i], 0x00FF0000, 0);
					}
				}
			}
		}
	}
}