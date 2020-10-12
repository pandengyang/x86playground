#include "stdio.h"
#include "global.h"

#if USE_INTERNAL_TASK

void init()
{
	static i32_t i = 0;

	while (1) {
		if (i++ % 1000000 == 0) {
			kputs("init...");
		}
	}
}

#endif
