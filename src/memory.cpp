#include <stddef.h>

extern "C" void memset(void* dest, int ch, size_t count) {
	for (size_t i = 0; i < count; ++i) {
		((unsigned char*) dest)[i] = (unsigned char) ch;
	}
}

extern "C" void memcpy(void* dest, const void* src, size_t count) {
	for (size_t i = 0; i < count; ++i) {
		((unsigned char*) dest)[i] = ((unsigned char*) src)[i];
	}
}