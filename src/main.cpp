#include "uefi/systemTable.hpp"
#include "uefi/protocols.hpp"
#include "elf.hpp"

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

	EFI_FILE_PROTOCOL* kernelFile;
	status = root->Open(&kernelFile, L"basickernel", EfiFileOpenMode::Read);
	if (status == EfiStatus::Success) {
		SystemTable->ConOut->OutputString(L"KernelFile\r\n");
	}

	Elf64Hdr hdr; // NOLINT(cppcoreguidelines-pro-type-member-init)
	uint64_t hdrSize = sizeof(Elf64Hdr);
	kernelFile->Read(&hdrSize, &hdr);

	kernelFile->SetPosition(hdr.e_phoff);

	for (size_t i = hdr.e_phoff; i < hdr.e_phoff + hdr.e_phnum * sizeof(Elf64PHdr); i += sizeof(Elf64PHdr)) {
		Elf64PHdr pHdr; // NOLINT(cppcoreguidelines-pro-type-member-init)
		uint64_t pHdrSize = sizeof(Elf64PHdr);
		kernelFile->SetPosition(i);
		kernelFile->Read(&pHdrSize, &pHdr);
		if (pHdr.p_type == PT_LOAD) {
			uint64_t offset = pHdr.p_vaddr - 0xffffffff80000000;
			uintptr_t buffer = 0x40000 + offset;
			SystemTable->BootServices->AllocatePages(EfiAllocateType::AllocateAddress, EfiMemoryType::LoaderData, pHdr.p_memsz, &buffer);
			kernelFile->SetPosition(pHdr.p_offset);
			kernelFile->Read(&pHdr.p_filesz, reinterpret_cast<void*>(buffer));
		}
	}

	kernelFile->Close();

	auto main = reinterpret_cast<__attribute__((sysv_abi)) int (*)(void)>(hdr.e_entry - 0xffffffff80000000 + 0x40000);
	auto result = main();
	if (result == 1) {
		SystemTable->ConOut->OutputString(L"kernel works! yay!!!! \r\n");
	}

	// --------------------------------------------------------------------------------------

	EFI_FILE_PROTOCOL* configFile;
	status = root->Open(&configFile, L"qbl.cfg", EfiFileOpenMode::Read);
	if (status == EfiStatus::Success) {
		SystemTable->ConOut->OutputString(L"ConfigFile\r\n");
	}

	EFI_GUID fileInfoGuid = EFI_FILE_INFO_ID;
	uint64_t bufferSize = sizeof(EFI_FILE_INFO) + 200 * sizeof(wchar_t);
	EFI_FILE_INFO* fileInfo;
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

	for (size_t i = 0; i < charBufferSize; ++i) {
		const wchar_t string[] = {static_cast<wchar_t>(charBuffer[i]), 0};
		SystemTable->ConOut->OutputString(string);
	}
	SystemTable->ConOut->OutputString(L"\r\n");
	SystemTable->ConOut->OutputString(L"file content outputted\r\n");

	// --------------------------------------------------------------------------------------

	uint64_t memoryMapSize;
	uint64_t descriptorSize;
	SystemTable->BootServices->GetMemoryMap(&memoryMapSize, nullptr, nullptr, &descriptorSize, nullptr);

	EFI_MEMORY_DESCRIPTOR* buffer;
	SystemTable->BootServices->AllocatePool(EfiMemoryType::LoaderData, memoryMapSize + 2 * descriptorSize,
	                                        reinterpret_cast<void**>(&buffer));

	while (status != EfiStatus::Success) {
		uint64_t mapKey;
		SystemTable->BootServices->GetMemoryMap(&memoryMapSize, buffer, &mapKey, &descriptorSize, nullptr);

		status = SystemTable->BootServices->ExitBootServices(ImageHandle, mapKey);
	}

	auto fb = gop->Mode->FrameBufferBase;
	for (size_t y = 0; y < 100; ++y) {
		for (size_t x = 0; x < 100; ++x) {
			*((uint32_t*) (fb + y * gop->Mode->Info->PixelsPerScanLine * 4 + x * 4)) = 0x0000FF00; // (reserved, r, g, b)
		}
	}

	while (true) asm("hlt");
}