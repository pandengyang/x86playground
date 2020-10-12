	SECTION CODE ALIGN=16 VSTART=07C00h

	[bits 16]
	; 初始值 cs: 00h eip: 07c00h
	mov ax, cs
	mov ds, ax
	mov ss, ax

	; |        | H
	; +--------+
	; |        | 引导扇区
	; +--------+ 07c00h 此处向上为引导扇区，向下为栈
	; |        | 栈
	; +--------+
	; |        | L
	mov sp, 07C00h

	mov ax, VIDEO_SEG
	mov es, ax

	; 清屏
	cld
	mov ax, 00720h
	mov edi, 0
	mov ecx, 1920
	rep stosw

	; 显示 boot...
	mov esi, boot_msg
	mov edi, 0
	mov ecx, BOOT_MSG_LEN
	rep movsb

	; 加载 GDT
	lgdt [cs:gdt_size]

	; 使能 A0
	in al, 092h
	or al, 0000_0010B
	out 092h, al

	; 关中断
	cli

	; 打开保护模式
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	; 强制使用新的 GDT
	jmp SEL_TEXT:dword flush

	[bits 32]
flush:
	; 初始化栈段
	mov eax, SEL_DATA
	mov ss, eax
	mov esp, STACK_TOP

	; 初始化数据段
	mov eax, SEL_DATA
	mov ds, eax
	mov es, eax

	; 显示 Prot...
	mov esi, prot_msg
	mov edi, VIDEO_START+160
	mov ecx, PROT_MSG_LEN
	rep movsb

	; 清空页表占用的内存
	mov al, 000h
	mov edi, PHY_ADDR_PAGE_DIR_TABLE ; 物理地址
	mov ecx, 0400000h
	rep stosb

	; 填充页目录项
	mov eax, PHY_ADDR_PAGE_TABLE_USER_1_4M
	or eax, PAGE_DIR_ENTRY_ATTR
	mov [PHY_ADDR_PAGE_DIR_TABLE + PAGE_DIR_TABLE_OFFSET_USER_1_4M], eax

	mov eax, PHY_ADDR_PAGE_TABLE_KERNEL_1_4M
	or eax, PAGE_DIR_ENTRY_ATTR
	mov [PHY_ADDR_PAGE_DIR_TABLE + PAGE_DIR_TABLE_OFFSET_KERNEL_1_4M], eax

	mov eax, PHY_ADDR_PAGE_TABLE_KERNEL_2_4M
	or eax, PAGE_DIR_ENTRY_ATTR
	mov [PHY_ADDR_PAGE_DIR_TABLE + PAGE_DIR_TABLE_OFFSET_KERNEL_2_4M], eax

	; 填充 0 ~ 4M 页表项
	; 该 4M 用于 boot 的运行
	mov ecx,  1024
	mov ebx, PHY_ADDR_PAGE_TABLE_USER_1_4M
	mov eax, PHY_ADDR_USER_1_4M
	or eax, PAGE_ENTRY_ATTR
.user_1_4m:
	mov [ebx], eax
	add ebx, 4
	add eax, 01000h ; 4K
	loop .user_1_4m

	; 填充 3G 开始的第 1 个 4m 页表项
	; 该 4m 用于存放页表
	mov ecx,  1024
	mov ebx, PHY_ADDR_PAGE_TABLE_KERNEL_1_4M
	mov eax, PHY_ADDR_KERNEL_1_4M
	or eax, PAGE_ENTRY_ATTR
.kernel_1_4m:
	mov [ebx], eax
	add ebx, 4
	add eax, 01000h ; 4K
	loop .kernel_1_4m

	; 填充 3G 开始的第 2 个 4m 页表项
	; 该 4m 用于放置内核
	mov ecx,  1024
	mov ebx, PHY_ADDR_PAGE_TABLE_KERNEL_2_4M
	mov eax, PHY_ADDR_KERNEL_2_4M
	or eax, PAGE_ENTRY_ATTR
.kernel_2_4m:
	mov [ebx], eax
	add ebx, 4
	add eax, 01000h ; 4K
	loop .kernel_2_4m

	; 指向页目录表
	mov eax, PHY_ADDR_PAGE_DIR_TABLE
	mov cr3, eax

        ; 开启页机制
	mov eax, cr0
	or eax, 080000000h
	mov cr0, eax

	; 从扇区 1 加载内核占用扇区数
	mov eax, 1
	mov ebx, VIR_ADDR_KERNEL_LOAD
	call read_hard_disk_0

	; 将内核占用扇区数赋值给 ecx
	mov ecx, [VIR_ADDR_KERNEL_LOAD]

	; 从扇区 2 开始的若干扇区加载内核
	mov eax, 2
	mov ebx, VIR_ADDR_KERNEL_LOAD
rdkernel:
	call read_hard_disk_0
	inc eax
	loop rdkernel

	; 内核为 ELF 格式
	; 程序头数量
	mov cx, [VIR_ADDR_KERNEL_LOAD + 02Ch]
	movzx ecx, cx

	; 程序头的位置
	mov ebx, [VIR_ADDR_KERNEL_LOAD + 01Ch]
	add ebx, VIR_ADDR_KERNEL_LOAD

	; 根据 program header 信息，将内核各部分放到合适的位置
.Move:
	mov eax, [ebx]

	; ecx 存储着程序头数量
	push ecx

	; 程序段是一个结构体
	; 第 4 个字节是 from
	; 第 8 个字节是 to
	mov esi, [ebx + 04h]
	add esi, VIR_ADDR_KERNEL_LOAD
	mov edi, [ebx + 08h]
	mov ecx, [ebx + 010h]
	rep movsb

	pop ecx

	add ebx, 020h
	dec ecx
	jnz .Move

	; 跳入内核
	jmp VIR_ADDR_KERNEL_ENTRY

; 读取主硬盘
; eax 起始扇区编号
; 数据写到ds:ebx 中
; ebx = ebx + 512
read_hard_disk_0:
	push eax
	push ecx
	push edx

	push eax

	; 读取一个扇区
	mov dx, 01F2h
	mov al, 1
	out dx, al

	pop eax

	; 起始扇区编号，LBA，主硬盘
	inc dx
	out dx, al

	inc dx
	mov cl, 8
	shr eax, cl
	out dx, al

	inc dx
	shr eax, cl
	out dx, al

	inc dx
	shr eax, cl
	or al, 0E0h
	out dx, al

	; 读硬盘
	inc dx
	mov al, 020h
	out dx, al

.waits:
	in al, dx
	and al, 088h
	cmp al, 008h
	jnz .waits

	; 读取数据到 ds:ebx 中
	mov ecx, 256
	mov dx, 01F0h
.readw:
	in ax, dx
	mov [ebx], ax
	add ebx, 2
	loop .readw

	pop edx
	pop ecx
	pop eax

	ret

	; Boot...
	boot_msg db 'B', 07h, 'o', 07h, 'o', 07h, 't', 07h, '.', 07h, '.', 07h, '.', 07h
	BOOT_MSG_LEN equ $-boot_msg

	; Prot...
	prot_msg db 'P', 07h, 'r', 07h, 'o', 07h, 't', 07h, '.', 07h, '.', 07h, '.', 07h
	PROT_MSG_LEN equ $-prot_msg

	VIDEO_SEG equ 0B800h
	VIDEO_START equ 0B8000h
	STACK_TOP equ 07C00h

GDT_BASE:
	dummy_desc db 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	text_desc db 0FFh, 0FFh, 00h, 00h, 00h, 098h, 0CFh, 00h
	data_desc db 0FFh, 0FFh, 00h, 00h, 00h, 092h, 0CFh, 00h

	GDT_SIZE equ $-GDT_BASE

	gdt_size dw GDT_SIZE
	gdt_base dd GDT_BASE

	SEL_TEXT equ 0008h
	SEL_DATA equ 0010h

	PAGE_DIR_ENTRY_ATTR equ 000000007h
	PAGE_ENTRY_ATTR equ 000000007h

	; 0 开始的 4m
	VIR_ADDR_USER_1_4M equ 00h
	; 3g 开始的第 1 个 4m
	VIR_ADDR_KERNEL_1_4M equ 0C0000000h
	; 3g 开始的第 2 个 4m
	VIR_ADDR_KERNEL_2_4M equ 0C0400000h

	; 0 开始的 4m 的物理地址 
	PHY_ADDR_USER_1_4M equ 00h
	; 3g 开始的第 1 个 4m 的物理地址
	PHY_ADDR_KERNEL_1_4M equ 0400000h
		PHY_ADDR_PAGE_DIR_TABLE equ 0400000h
		PHY_ADDR_PAGE_TABLE_USER_1_4M equ 0401000h
		PHY_ADDR_PAGE_TABLE_KERNEL_1_4M equ 0402000h
		PHY_ADDR_PAGE_TABLE_KERNEL_2_4M equ 0403000h
	; 3g 开始的第 2 个 4m 的物理地址
	PHY_ADDR_KERNEL_2_4M equ 0800000h

	; 页目录表项的偏移
	PAGE_DIR_TABLE_OFFSET_USER_1_4M equ 00h
	PAGE_DIR_TABLE_OFFSET_KERNEL_1_4M equ 0C00h
	PAGE_DIR_TABLE_OFFSET_KERNEL_2_4M equ 0C04h

	; kernel 加载在此处，但根据 ELF 中的信息放在 3g 开始的第 2 个 4m
	VIR_ADDR_KERNEL_LOAD equ 07E00h
	; kernel 入口地址，与内核链接时指定的入口一致
	VIR_ADDR_KERNEL_ENTRY equ 0C0400000h

	times 510-($-$$) db 0
	db 0x55, 0xaa
