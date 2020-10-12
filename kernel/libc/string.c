#include "x86playground/type.h"
#include "string.h"

void *memcpy(void *pdest, void *psrc, u32_t count)
{
	u8_t *pcdest = (u8_t *) pdest;
	u8_t *pcsrc = (u8_t *) psrc;

	for (i32_t i = 0; i < count; i++) {
		*pcdest = *pcsrc;
		pcdest++;
		pcsrc++;
	}
}

char *strcpy(char *dest, const char *src)
{
	for (; *src != '\0'; src++, dest++) {
		*dest = *src;
	}
	*dest = '\0';

	return dest;
}

u32_t strlen(const char *str)
{
	const char *end;

	for (end = str; *end != '\0'; end++) {
	}

	return end - str;
}
