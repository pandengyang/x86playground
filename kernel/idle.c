#include "global.h"
#include "stdio.h"

#if USE_INTERNAL_TASK

void idle()
{
	static i32_t i = 0;

	while (1) {
		if (i++ % 1000000 == 0) {
			kputs("idle...");
		}
	}
}

#endif
