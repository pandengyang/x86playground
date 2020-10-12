	global read_hard_disk_0

read_hard_disk_0:
	; 保护用到的寄存器
	push eax
	push ebx
	push ecx
	push edx

	; read_hard_disk_0(sector_dest_num, dest);
	; +-----------------+
	; | dest            | esp + 24
	; +-----------------+
	; | sector_dest_num | esp + 20
	; +-----------------+
	; | ret_addr        | esp + 16
	; +-----------------+
	; | eax             | esp + 12
	; +-----------------+
	; | ebx             | esp + 8
	; +-----------------+
	; | ecx             | esp + 4
	; +-----------------+
	; | edx             | esp
	; +-----------------+
	;
	; sector_dest_num -> eax
	; dest -> ebx
	;
	; eax 起始扇区编号
	; 数据写到 ds:ebx 中
	mov eax, [esp + 20]
	mov ebx, [esp + 24]

	; 读取一个扇区
	push eax
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
	pop ebx
	pop eax

	ret
