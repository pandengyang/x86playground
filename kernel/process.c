#include "global.h"
#include "string.h"

#if USE_INTERNAL_TASK

u8_t stack_task[SIZE_TASK_STACK * NR_TASKS];

void init();
void idle();

#else

void read_hard_disk_0(u32_t sector_start_num, u8_t * dest);

/* init 任务占据从扇区 100 开始的 16 个扇区 */
#define SECTOR_START_NUM_INIT 100
/* idle 任务占据从扇区 116 开始的 16 个扇区 */
#define SECTOR_START_NUM_IDLE 116
/* init 与 idle 任务占据的扇区数 */
#define SECTOR_NUM 16
/* 扇区大小 */
#define SECTOR_SIZE 512

/* 内核加载任务后，存储到此处 */
u8_t buf_task[SECTOR_SIZE * SECTOR_NUM];

/* 任务加载后，被放到 0x08000000 开始的 4m 空间
 * 从该 4m 空间中选择一块区域作为该任务的栈
 */
#define STACK_TOP_TASK 0x08300000

#endif

void restart();

static void init_tasks();
static void init_pcbs();
static void init_tss();

void init_process()
{
	init_tasks();
	init_pcbs();
	init_tss();

	/* 令 pcbs 表中的第一个进程作为起始进程 */
	proc_next = pcbs;
	/* 切换到起始进程的页表 */
	proc_next->switch_pt();

	restart();
}

static void init_tasks()
{
	tasks[0].pid = ++pid_next;
	tasks[1].pid = ++pid_next;

#if USE_INTERNAL_TASK
	tasks[0].entry = (void *) init;
	tasks[1].entry = (void *) idle;
#else
	Elf32_Ehdr *eheader;
	Elf32_Phdr *pheader;

	/* 从硬盘读取 init 程序 */
	for (i8_t i = 0; i < SECTOR_NUM; i++) {
		read_hard_disk_0(SECTOR_START_NUM_INIT + i,
				 buf_task + i * SECTOR_SIZE);
	}

	/* 定位 init 的 ELF header */
	eheader = (Elf32_Ehdr *) buf_task;

	/* 指定 init 的入口地址 */
	tasks[0].entry = (void *) eheader->e_entry;

	/* 指定使用 init 的用户态页表 */
	tasks[0].switch_pt = set_page_task_init_4m;
	tasks[0].switch_pt();

	/* 将 init 各程序段放在合适的虚拟内存上 */
	pheader = (Elf32_Phdr *) (buf_task + eheader->e_phoff);
	for (i16_t i = 0; i < eheader->e_phnum; i++) {
		memcpy((void *) pheader->p_vaddr,
		       buf_task + pheader->p_offset, pheader->p_filesz);
		pheader = pheader + 1;
	}

	/* 从硬盘读取 idle 程序 */
	for (i8_t i = 0; i < SECTOR_NUM; i++) {
		read_hard_disk_0(SECTOR_START_NUM_IDLE + i,
				 buf_task + i * SECTOR_SIZE);
	}

	/* 定位 idle 的 ELF header */
	eheader = (Elf32_Ehdr *) buf_task;

	/* 指定 idle 的入口地址 */
	tasks[1].entry = (void *) eheader->e_entry;

	/* 指定使用 idle 的用户态页表 */
	tasks[1].switch_pt = set_page_task_idle_4m;
	tasks[1].switch_pt();

	/* 将 idle 各程序段放在合适的虚拟内存上 */
	pheader = (Elf32_Phdr *) (buf_task + eheader->e_phoff);
	for (i16_t i = 0; i < eheader->e_phnum; i++) {
		memcpy((void *) pheader->p_vaddr,
		       buf_task + pheader->p_offset, pheader->p_filesz);
		pheader = pheader + 1;
	}
#endif
	strcpy(tasks[0].name, "init");
	strcpy(tasks[1].name, "idle");
}

static void init_pcbs()
{
	pcb_t *p;

	for (i32_t i = 0; i < NR_TASKS; i++) {
		p = pcbs + i;

		p->ldt_sel = SEL_INIT_G + i * 8;

#if USE_INTERNAL_TASK
		/* 任务编译进内核时，任务在 3g 以上空间
		 * 需要任务可以访问全部 4g 空间
		 */
		init_ldesc(p->ldts, 0x00000000, 0xFFFFF, LDT_ATTR_TEXT);
		init_ldesc(p->ldts + 1, 0x00000000, 0xFFFFF,
			   LDT_ATTR_DATA);
#else
		/* 任务单独编译时，任务在 3g 以下空间
		 * 需要任务可以访问 0 ~ 3g 空间
		 */
		init_ldesc(p->ldts, 0x00000000, 0xBFFFF, LDT_ATTR_TEXT);
		init_ldesc(p->ldts + 1, 0x00000000, 0xBFFFF,
			   LDT_ATTR_DATA);
#endif
		/* 在 GDT 中添加任务的 LDT 的描述符 */
		init_gdesc(gdt + IDX_INIT_G + i, (u32_t) (p->ldts),
			   (u32_t) (sizeof p->ldts - 1), GDT_ATTR_LDT);

		/* 初始化任务的栈帧 */
		p->regs.cs = SEL_TEXT_L;
		p->regs.ds = SEL_DATA_L;
		p->regs.es = SEL_DATA_L;
		p->regs.fs = SEL_DATA_L;
		p->regs.ss = SEL_DATA_L;
		p->regs.gs = SEL_DATA_L;
		p->regs.eip = (u32_t) tasks[i].entry;
		p->regs.eflags = EFLAGS;
#if USE_INTERNAL_TASK
		/* 使用内核的数组作为栈 */
		p->regs.esp =
		    (u32_t) stack_task + SIZE_TASK_STACK * (i + 1);
#else
		/* 使用任务所在 4M 的一部分空间作为栈 */
		p->regs.esp = STACK_TOP_TASK;

		p->switch_pt = tasks[i].switch_pt;
#endif

		p->pid = tasks[i].pid;
		p->ticks = 0;
		strcpy(p->name, tasks[i].name);
	}
}

static void init_tss()
{
	/* 初始化 tss 信息，只需指定 ring0 的栈 */
	tss.ss0 = SEL_DATA_G;
	tss.iobase = sizeof tss;

	/* 在 GDT 中为 TSS 添加描述符 */
	init_gdesc(gdt + IDX_TSS_G, (u32_t) & tss, sizeof tss - 1,
		   GDT_ATTR_TSS);
}

void schedule()
{
	/* 每个任务执行 MAX_PROC_TICKS 个时钟滴答后，被剥夺执行权 */
	if (proc_next->ticks > MAX_PROC_TICKS) {
		proc_next->ticks = 0;
		proc_next++;

		if (proc_next >= pcbs + NR_TASKS) {
			proc_next = pcbs;
		}
	}
}
