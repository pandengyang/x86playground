#ifndef _PAGE_H_
#define _PAGE_H_

typedef unsigned int line_addr;
typedef unsigned int phy_addr;

typedef unsigned int page_dir_entry;
typedef unsigned int page_entry;

#define PAGE_DIR_ENTRY_OFFSET 22

#define PAGE_DIR_ENTRY_ATTR 0x00000007
#define PAGE_ENTRY_ATTR 0x00000007

#define VIR_ADDR_KERNEL_1_4M 0xC0000000 /* page tables */
	#define VIR_ADDR_PAGE_DIR_TABLE 0xC0000000 /* page dir table */
	#define VIR_ADDR_PAGE_TABLE_KERNEL_1_4M 0xC0002000 /* page table for kernel_1_4m */
	#define VIR_ADDR_PAGE_TABLE_KERNEL_2_4M 0xC0003000 /* page table for kernel_2_4m */
	#define VIR_ADDR_PAGE_TABLE_KERNEL_3_4M 0xC0004000 /* page table for kernel_3_4m */
	#define VIR_ADDR_PAGE_TABLE_KERNEL_256_4M 0xC0005000 /* page table for kernel_256_4m */
	#define VIR_ADDR_PAGE_TABLE_TASK_4M 0xC0006000 /* page table for task_4m */
#define VIR_ADDR_KERNEL_2_4M 0xC0400000 /* kernel */
#define VIR_ADDR_KERNEL_3_4M 0xC0800000 /* video */
#define VIR_ADDR_KERNEL_256_4M 0xFFC00000 /* stack */
#define VIR_ADDR_TASK_4M 0x08000000 /* task */

#define PHY_ADDR_KERNEL_1_4M 0x00000000 /* video */
#define PHY_ADDR_KERNEL_2_4M 0x00400000 /* page */
	#define PHY_ADDR_PAGE_DIR_TABLE 0x00400000 /* page dir table */
	#define PHY_ADDR_PAGE_TABLE_KERNEL_1_4M 0x00402000 /* page table for kernel_1_4m */
	#define PHY_ADDR_PAGE_TABLE_KERNEL_2_4M 0x00403000 /* page table for kernel_2_4m */
	#define PHY_ADDR_PAGE_TABLE_KERNEL_3_4M 0x00404000 /* page table for kernel_3_4m */
	#define PHY_ADDR_PAGE_TABLE_KERNEL_256_4M 0x00405000 /* page table for kernel_256_4m */
	#define PHY_ADDR_PAGE_TABLE_TASK_4M 0x00406000 /* page table for task_4m */
#define PHY_ADDR_KERNEL_3_4M 0x00800000 /* kernel */
#define PHY_ADDR_KERNEL_256_4M 0x00C00000 /* stack */
#define PHY_ADDR_TASK_INIT_4M 0x01000000 /* init */
#define PHY_ADDR_TASK_IDLE_4M 0x01400000 /* idle */

void set_page_task_init_4m();
void set_page_task_idle_4m();

#endif
