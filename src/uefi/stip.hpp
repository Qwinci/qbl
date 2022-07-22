#pragma once
#include "types.hpp"

#define CHAR_BACKSPACE 0x8
#define CHAR_TAB 0x9
#define CHAR_LINEFEED 0xA
#define CHAR_CARRIAGE_RETURN 0xD

#define SCAN_NULL 0x0
#define SCAN_UP 0x1
#define SCAN_DOWN 0x2
#define SCAN_RIGHT 0x3
#define SCAN_LEFT 0x4
#define SCAN_HOME 0x5
#define SCAN_END 0x6
#define SCAN_INSERT 0x7
#define SCAN_DELETE 0x8
#define SCAN_PAGE_UP 0x9
#define SCAN_PAGE_DOWN 0xA
#define SCAN_F1 0xB
#define SCAN_F2 0xC
#define SCAN_F3 0xD
#define SCAN_F4 0xE
#define SCAN_F5 0xF
#define SCAN_F6 0x10
#define SCAN_F7 0x11
#define SCAN_F8 0x12
#define SCAN_F9 0x13
#define SCAN_F10 0x14
#define SCAN_ESC 0x17

struct EFI_INPUT_KEY {
	UINT16 ScanCode;
	UINT16 UnicodeChar;
};

struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
	inline EFIAPI EfiStatus Reset(BOOLEAN ExtendedVerification) {
		return Reset_(this, ExtendedVerification);
	}
	inline EFIAPI EfiStatus ReadKeyStroke(EFI_INPUT_KEY* Key) {
		return ReadKeyStroke_(this, Key);
	}
private:
	EfiStatus (EFIAPI* Reset_)(IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL* This,
	IN BOOLEAN ExtendedVerification);
	EfiStatus (EFIAPI* ReadKeyStroke_)(IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL* This,
	OUT EFI_INPUT_KEY* Key);
	EFI_EVENT WaitForKey;
};