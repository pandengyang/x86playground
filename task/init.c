#include "x86playground/type.h"

i32_t puts(const char *s);

void main()
{
	static i32_t i = 0;

	while (1) {
		if (i++ % 1000000 == 0) {
			puts("init...");
		}
	}
}
