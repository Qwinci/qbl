#pragma once
#include "types.hpp"

struct EFI_SIMPLE_TEXT_OUTPUT_MODE {
	INT32 MaxMode;
	INT32 Mode;
	INT32 Attribute;
	INT32 CursorColumn;
	INT32 CursorRow;
	BOOLEAN CursorVisible;
};

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
	inline EFIAPI EfiStatus Reset(BOOLEAN ExtendedVerification) {
		return Reset_(this, ExtendedVerification);
	}
	inline EFIAPI EfiStatus OutputString(const CHAR16* String) {
		return OutputString_(this, String);
	}
	inline EFIAPI EfiStatus TestString(const CHAR16* String) {
		return TestString_(this, String);
	}
	inline EFIAPI EfiStatus QueryMode(UINTN ModeNumber, UINTN* Columns, UINTN* Rows) {
		return QueryMode_(this, ModeNumber, Columns, Rows);
	}
	inline EFIAPI EfiStatus SetMode(UINTN ModeNumber) {
		return SetMode_(this, ModeNumber);
	}
	inline EFIAPI EfiStatus SetAttribute(UINTN Attribute) {
		return SetAttribute_(this, Attribute);
	}
	inline EFIAPI EfiStatus ClearScreen() {
		return ClearScreen_(this);
	}
	inline EFIAPI EfiStatus SetCursorPosition(UINTN Column, UINTN Row) {
		return SetCursorPosition_(this, Column, Row);
	}
	inline EFIAPI EfiStatus EnableCursor(BOOLEAN Visible) {
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