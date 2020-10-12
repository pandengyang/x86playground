	[BITS 32]
	[SECTION .text]

	; 导出全局符号
	global _start

	global divide_error
	global single_step_exception
	global nmi
	global breakpoint_exception
	global overflow
	global bounds_check
	global inval_opcode
	global copr_not_available
	global double_fault
	global copr_seg_overrun
	global inval_tss
	global segment_not_present
	global stack_exception
	global general_protection
	global page_fault
	global copr_error
	global align_check
	global machine_check
	global smd_float_exception

	global irq020_handler
	global irq090_handler

	global restart

	; 外部符号
	extern set_page_kernel_3_4m
	extern set_page_kernel_256_4m

	extern gdt_info
	extern init_gdt

	extern exception_handler
	extern clock_handler
	extern syscall_handler
	extern init_idt
	extern idt_info
	extern init_8259a
	extern reenable_8259a

	extern tip_kernel
	extern tip_halt
	extern tip_irq0

	extern tss
	extern proc_next
	extern init_process

_start:
	; 设置 Video 虚拟页
	call set_page_kernel_3_4m

	; 设置内核栈虚拟页
	call set_page_kernel_256_4m
	mov esp, 000h

	; Debug，显示 Kernel...
	call tip_kernel

	; 将 GDT 挪到内核空间
	sgdt [gdt_info]
	call init_gdt
	lgdt [gdt_info]

	; 强制使用新的 GDT
	jmp SEL_TEXT_G:flush

flush:
	; 设置 IDT
	call init_idt
	lidt [idt_info]

	; 通过编程 8259A 建立可屏蔽中断与 CPU 关系
	call init_8259a

	; 开/关中断
	; sti
	cli

	; Debug，除法错异常
	; mov ebx, 0 
	; div ebx

	; 初始化进程环境
	jmp init_process

	; 从 ring0 跳到 ring3
restart:
	; 加载任务
	mov ax, SEL_TSS_G
	ltr ax

	; 使用当前进程栈帧作为栈，以弹出栈帧中的寄存器
	mov esp , [proc_next]

	; 加载 LDT
	lldt [esp + PCB_OFFSET_LDT_SEL]

	; tss 中的 ss0 是固定的
	; tss 中 esp0 指向当前任务的栈帧
	; 这样发生中断时，在栈帧中自动保存寄存器
	lea eax, [esp + PCB_OFFSET_STACKTOP]
	mov dword [tss + TSS_OFFSET_ESP0], eax

	pop gs
	pop fs
	pop es
	pop ds
	popad

	iretd

	; 停机
halt:
	; Debug，显示 halt...
	call tip_halt

	hlt
	jmp halt

	; 异常处理函数
	; error_code vector_no
divide_error:
	push 0xFFFFFFFF
	push 0
	jmp exception

single_step_exception:
	push 0xFFFFFFFF
	push 1
	jmp exception

nmi:
	push 0xFFFFFFFF
	push 2
	jmp exception

breakpoint_exception:
	push 0xFFFFFFFF
	push 3
	jmp exception

overflow:
	push 0xFFFFFFFF
	push 4
	jmp exception

bounds_check:
	push 0xFFFFFFFF
	push 5
	jmp exception

inval_opcode:
	push 0xFFFFFFFF
	push 6
	jmp exception

copr_not_available:
	push 0xFFFFFFFF
	push 7
	jmp exception

double_fault:
	push 8
	jmp exception

copr_seg_overrun:
	push 0xFFFFFFFF
	push 9
	jmp exception

inval_tss:
	push 10
	jmp exception

segment_not_present:
	push 11
	jmp exception

stack_exception:
	push 12
	jmp exception

general_protection:
	push 13
	jmp exception

page_fault:
	push 14
	jmp exception

copr_error:
	push 0xFFFFFFFF
	push 16
	jmp exception

align_check:
	push 17
	jmp exception

machine_check:
	push 0xFFFFFFFF
	push 18
	jmp exception

smd_float_exception:
	push 0xffffffff
	push 18
	jmp exception

	; 公共异常处理代码
exception:
	call exception_handler
	add esp, 8

	; 异常后宕机
down:
	hlt
	jmp down

	; 时钟中断处理，显示 irq0...
; irq020_handler:
;	call tip_irq0
;	call reenable_8259a

;	iretd

; 时钟中断处理函数
irq020_handler:
	; 屏蔽中断，中断处理期间不允许中断重入
	cli

	; 在栈帧中存储寄存器：ss esp eflags cs eip
	; 硬件自动进行

	; pushad 保存寄存器：eax ecx edx ebx esp ebp ebp esi edi
	pushad

	push ds
	push es
	push fs
	push gs

	; 使用内核段
	mov dx, ss
	mov ds, dx
	mov es, dx

	; 使用内核栈进行函数调用
	mov esp, VIR_ADDR_KERNEL_STACK_TOP

	call clock_handler

	; 进入时钟中断后，时钟中断被屏蔽，从中断处理函数退出时需重新使能时钟中断
	call reenable_8259a

	; 使用当前进程栈帧作为栈，以弹出栈帧中的寄存器
	mov esp, [proc_next]

	; 加载 LDT
	lldt [esp + PCB_OFFSET_LDT_SEL]

	; tss 中的 ss0 是固定的
	; tss 中 esp0 指向当前进行的栈帧
	; 这样发生中断时，在栈帧中进行自动保存
	lea eax, [esp + PCB_OFFSET_STACKTOP]
	mov dword [tss + TSS_OFFSET_ESP0], eax

	pop gs
	pop fs
	pop es
	pop ds
	popad

	; 开中断
	sti

	iretd

; int 090h 的中断处理函数，该中断为系统调用
irq090_handler:
	; 屏蔽中断，中断处理期间不允许中断重入
	cli

	; 在栈帧中存储寄存器：ss esp eflags cs eip
	; 硬件自动进行

	; pushad 保存寄存器：eax ecx edx ebx esp ebp ebp esi edi
	pushad

	push ds
	push es
	push fs
	push gs

	; 使用内核段
	mov dx, ss
	mov ds, dx
	mov es, dx

	; 使用内核栈进行函数调用
	mov esp, VIR_ADDR_KERNEL_STACK_TOP

	; eax 为系统调用号
	; ebx 为参数
	push ebx
	push eax
	call syscall_handler
	add esp, 8

	; 使用当前进程栈帧作为栈，以弹出栈帧中的寄存器
	mov esp, [proc_next]

	; 加载 LDT
	lldt [esp + PCB_OFFSET_LDT_SEL]

	; tss 中的 ss0 是固定的
	; tss 中 esp0 指向当前进行的栈帧
	; 这样发生中断时，在栈帧中进行自动保存
	lea eax, [esp + PCB_OFFSET_STACKTOP]
	mov dword [tss + TSS_OFFSET_ESP0], eax

	pop gs
	pop fs
	pop es
	pop ds
	popad

	; 开中断
	sti

	iretd

	; 代码段选择子
	SEL_TEXT_G equ 0008h
	SEL_TSS_G equ 0018h

	; 进程控制块字段偏移
	PCB_OFFSET_LDT_SEL equ 68
	PCB_OFFSET_STACKTOP equ 68

	; TSS 字段偏移
	TSS_OFFSET_ESP0 equ 4

	; 内核栈顶
	VIR_ADDR_KERNEL_STACK_TOP equ 0xFFFFFFFF
