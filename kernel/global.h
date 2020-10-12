#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "x86playground/type.h"
#include "x86playground/segment.h"
#include "x86playground/process.h"
#include "x86playground/page.h"
#include "x86playground/elf32.h"
#include "x86playground/syscall.h"

#define EXTERN extern

#ifdef TABLE
	#undef EXTERN
	#define EXTERN
#endif

EXTERN i32_t tty_pos;

EXTERN u8_t gdt_info[6];
EXTERN gdesc_t gdt[GDT_SIZE];

EXTERN u8_t idt_info[6];
EXTERN gate_t idt[IDT_SIZE];

EXTERN u32_t pid_next;
EXTERN task_t tasks [NR_TASKS];
EXTERN pcb_t pcbs[NR_TASKS];
EXTERN pcb_t *proc_next;
EXTERN tss_t tss;

EXTERN syscall_func syscalls[SYSCALL_NUM];

void schedule();

/* 使用编译进内核内部的 TASK 还是从硬盘上加载的 TASK */
#define USE_INTERNAL_TASK 0

#endif
