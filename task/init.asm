	[BITS 32]
	[SECTION .text]

	; 导出全局符号
	global _start

	extern main

; 当使用 gcc 进行编译，main 作为入口函数时，gcc 会添加很多代码，这些代码脱离 glibc 无法运行
; 因此使用我们自己定义的入口函数
_start:
	jmp main
