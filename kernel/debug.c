#include "global.h"
#include "stdio.h"

void tip_kernel()
{
	kputs("Kernel...");
}

void tip_halt()
{
	kputs("halt...");
}

void tip_irq0()
{
	static i32_t i = 0;

	if (i++ % 1000 == 0) {
		kputs("irq 0...");
	}
}
