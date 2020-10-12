#include "global.h"
#include "stdio.h"
#include "string.h"

/* 中断处理函数在 kernel.asm 中定义
 * 以下为异常处理函数
 */
void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void page_fault();
void copr_error();
void align_check();
void machine_check();
void smd_float_exception();

/* 时钟中断处理函数 */
void irq020_handler();

/* int 090h 中断处理函数 */
void irq090_handler();

static void install_syscall();

void init_idt()
{
	/* 异常处理函数 */
	init_gate(idt, SEL_TEXT_G, (u32_t) divide_error,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 1, SEL_TEXT_G, (u32_t) single_step_exception,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 2, SEL_TEXT_G, (u32_t) nmi, IDT_ATTR_INT_GATE);
	init_gate(idt + 3, SEL_TEXT_G,
		  (u32_t) breakpoint_exception, IDT_ATTR_INT_GATE);
	init_gate(idt + 4, SEL_TEXT_G, (u32_t) overflow,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 5, SEL_TEXT_G, (u32_t) bounds_check,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 6, SEL_TEXT_G, (u32_t) inval_opcode,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 7, SEL_TEXT_G, (u32_t) copr_not_available,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 8, SEL_TEXT_G, (u32_t) double_fault,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 9, SEL_TEXT_G, (u32_t) copr_seg_overrun,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 10, SEL_TEXT_G, (u32_t) inval_tss,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 11, SEL_TEXT_G, (u32_t) segment_not_present,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 12, SEL_TEXT_G, (u32_t) stack_exception,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 13, SEL_TEXT_G, (u32_t) general_protection,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 14, SEL_TEXT_G, (u32_t) page_fault,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 16, SEL_TEXT_G, (u32_t) copr_error,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 17, SEL_TEXT_G, (u32_t) align_check,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 18, SEL_TEXT_G, (u32_t) machine_check,
		  IDT_ATTR_INT_GATE);
	init_gate(idt + 19, SEL_TEXT_G, (u32_t) smd_float_exception,
		  IDT_ATTR_INT_GATE);

	/* 时钟中断处理函数 */
	init_gate(idt + 0x20, SEL_TEXT_G, (u32_t) irq020_handler,
		  IDT_ATTR_INT_GATE);

	/* 系统调用 int 090h 处理函数 */
	init_gate(idt + 0x90, SEL_TEXT_G, (u32_t) irq090_handler,
		  IDT_ATTR_INT_GATE);

	/* 使用 idt 变量的信息更新 idt_info */
	u16_t *plimit = (u16_t *) idt_info;
	u32_t *pbase = (u32_t *) (idt_info + 2);
	*pbase = (u32_t) & idt;
	*plimit = sizeof idt - 1;

	// 安装系统调用
	install_syscall();
}

static void install_syscall()
{
	syscalls[SYSCALL_NUM_PUTS] = kputs;
}

/* 异常处理函数仅仅将异常打印出来 */
void exception_handler(i32_t vector_no, i32_t errcode, i32_t eip, i32_t cs,
		       i32_t eflags)
{
	char *errmsg[] = {
		"#DE",
		"#DB",
		"NMI",
		"#BP",
		"#OF",
		"#BR",
		"#UD",
		"#NM",
		"#DF",
		"#CSO",
		"#TS",
		"#NP",
		"#SS",
		"#GP",
		"#PF",
		"#Reserved",
		"#MF",
		"#AC",
		"#MC",
		"#XF",
	};

	kputs(errmsg[vector_no]);
}

void clock_handler()
{
	/* 时钟滴答加一 */
	proc_next->ticks++;

	/* 进程调度 */
	schedule();

	/* 切换到新进程的页表 */
	proc_next->switch_pt();
}

void syscall_handler(u32_t syscall_num, void *arg1)
{
	/* 所有的系统调用均由 int 090h 发起，具体是哪个系统调用，由系统调用号决定 */
	syscalls[syscall_num] (arg1);
}
