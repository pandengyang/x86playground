	[BITS 32]
	[SECTION .text]

	; 导出全局符号
	global puts

; 使用系统调用在屏幕进行输出
puts:
	push eax
	push ebx

	; eax 为系统调用号
	; ebx 为参数
	mov eax, SYSCALL_NUM_PUTS
	mov ebx, [esp + 12]
	int SYSCALL_IRQ_NUM

	pop ebx
	pop eax

	ret

	SYSCALL_IRQ_NUM equ 090h
	SYSCALL_NUM_PUTS equ 00h
