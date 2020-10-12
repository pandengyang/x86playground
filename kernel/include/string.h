#ifndef _STRING_H_
#define _STRING_H_

#include "x86playground/type.h"

void *memcpy (void *pdest, void *psrc, u32_t count);
char *strcpy (char *dest, const char *src);

u32_t strlen (const char *str);

#endif
