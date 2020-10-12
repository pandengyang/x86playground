#include "global.h"
#include "string.h"

void init_gdt()
{
	/* 此时的 gdt_info 描述的是引导扇区中的 GDT */
	u16_t *plimit = (u16_t *) gdt_info;
	u32_t *pbase = (u32_t *) (gdt_info + 2);

	/* 将引导扇区中的 GDT 拷贝到 gdt 变量中 */
	memcpy((void *) &gdt, (void *) *pbase, *plimit + 1);

	/* 使用 gdt 变量的信息更新 gdt_info */
	*pbase = (u32_t) & gdt;
	*plimit = sizeof gdt - 1;

	return;
}
