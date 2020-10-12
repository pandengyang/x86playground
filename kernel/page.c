#include "global.h"

void invalid_tlb();

/* 设置从 3g 开始的第三个 4m 的页表，Video 放在这里 */
void set_page_kernel_3_4m()
{
	page_dir_entry *page_dir_entries;
	page_entry *page_entries;

	page_dir_entries = (page_dir_entry *) VIR_ADDR_PAGE_DIR_TABLE;
	page_dir_entries[VIR_ADDR_KERNEL_3_4M >>
			 PAGE_DIR_ENTRY_OFFSET] =
	    PHY_ADDR_PAGE_TABLE_KERNEL_3_4M | PAGE_DIR_ENTRY_ATTR;

	page_entries = (page_entry *) VIR_ADDR_PAGE_TABLE_KERNEL_3_4M;
	for (i32_t i = 0; i < 1024; i++) {
		page_entries[i] =
		    (PHY_ADDR_KERNEL_1_4M + i * 4096) | PAGE_ENTRY_ATTR;
	}
}

/* 设置从 4g 地址空间的最后一个 4m 的页表，此处为内核栈 */
void set_page_kernel_256_4m()
{
	page_dir_entry *page_dir_entries;
	page_entry *page_entries;

	page_dir_entries = (page_dir_entry *) VIR_ADDR_PAGE_DIR_TABLE;
	page_dir_entries[VIR_ADDR_KERNEL_256_4M >>
			 PAGE_DIR_ENTRY_OFFSET] =
	    PHY_ADDR_PAGE_TABLE_KERNEL_256_4M | PAGE_DIR_ENTRY_ATTR;

	page_entries = (page_entry *) VIR_ADDR_PAGE_TABLE_KERNEL_256_4M;
	for (i32_t i = 0; i < 1024; i++) {
		page_entries[i] =
		    (PHY_ADDR_KERNEL_256_4M + i * 4096) | PAGE_ENTRY_ATTR;
	}
}

/* 设置 init 任务占用的 4m 空间的表，init 与 idle 任务所占用 4m 空间虚拟地址相同，物理地址不同 */
void set_page_task_init_4m()
{
	page_dir_entry *page_dir_entries;
	page_entry *page_entries;

	page_dir_entries = (page_dir_entry *) VIR_ADDR_PAGE_DIR_TABLE;
	page_dir_entries[VIR_ADDR_TASK_4M >>
			 PAGE_DIR_ENTRY_OFFSET] =
	    PHY_ADDR_PAGE_TABLE_TASK_4M | PAGE_DIR_ENTRY_ATTR;

	page_entries = (page_entry *) VIR_ADDR_PAGE_TABLE_TASK_4M;
	for (i32_t i = 0; i < 1024; i++) {
		page_entries[i] =
		    (PHY_ADDR_TASK_INIT_4M + i * 4096) | PAGE_ENTRY_ATTR;
	}

	/* 进程切换时更新页表，需使 TLB 无效，页表才能起作用 */
	invalid_tlb();
}

/* 设置 idle 任务占用的 4m 空间的表，init 与 idle 任务所占用 4m 空间虚拟地址相同，物理地址不同 */
void set_page_task_idle_4m()
{
	page_dir_entry *page_dir_entries;
	page_entry *page_entries;

	page_dir_entries = (page_dir_entry *) VIR_ADDR_PAGE_DIR_TABLE;
	page_dir_entries[VIR_ADDR_TASK_4M >>
			 PAGE_DIR_ENTRY_OFFSET] =
	    PHY_ADDR_PAGE_TABLE_TASK_4M | PAGE_DIR_ENTRY_ATTR;

	page_entries = (page_entry *) VIR_ADDR_PAGE_TABLE_TASK_4M;
	for (i32_t i = 0; i < 1024; i++) {
		page_entries[i] =
		    (PHY_ADDR_TASK_IDLE_4M + i * 4096) | PAGE_ENTRY_ATTR;
	}

	/* 进程切换时更新页表，需使 TLB 无效，页表才能起作用 */
	invalid_tlb();
}
