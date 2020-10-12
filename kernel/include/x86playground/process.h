#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "x86playground/type.h"
#include "x86playground/segment.h"

#define NR_TASKS 2
#define SIZE_TASK_STACK 1024

/* 每个任务最多执行 100 个时钟滴答，然后会被抢占 */
#define MAX_PROC_TICKS 100

/* 从 ring0 跳转到 ring3 时是否开启中断 */
// #define EFLAGS 0x1002
#define EFLAGS 0x1202

typedef struct {
	u32_t gs;
	u32_t fs;
	u32_t es;
	u32_t ds;
	u32_t edi;
	u32_t esi;
	u32_t ebp;
	u32_t kernel_esp;
	u32_t ebx;
	u32_t edx;
	u32_t ecx;
	u32_t eax;
	u32_t eip;
	u32_t cs;
	u32_t eflags;
	u32_t esp;
	u32_t ss;
} stackframe_t;

typedef struct {
	stackframe_t regs;
	u16_t ldt_sel;
	ldesc_t ldts[LDT_SIZE];
	u32_t pid;
	u32_t ticks;
	char name[16];
	void (*switch_pt)(void);
} pcb_t;

typedef struct {
	u32_t backlink;
	u32_t esp0;
	u32_t ss0;
	u32_t esp1;
	u32_t ss1;
	u32_t esp2;
	u32_t ss2;
	u32_t cr3;
	u32_t eip;
	u32_t flags;
	u32_t eax;
	u32_t ecx;
	u32_t edx;
	u32_t ebx;
	u32_t esp;
	u32_t ebp;
	u32_t esi;
	u32_t edi;
	u32_t es;
	u32_t cs;
	u32_t ss;
	u32_t ds;
	u32_t fs;
	u32_t gs;
	u32_t ldt;
	u16_t trap;
	u16_t iobase;
} tss_t;

typedef struct {
	u32_t pid;
	char name[16];
	void *entry;
	void (*switch_pt)(void);
} task_t;

void schedule();

#endif
