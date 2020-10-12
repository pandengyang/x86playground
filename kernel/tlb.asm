	global invalid_tlb

; 使 TLB 失效
invalid_tlb:
	push eax

	; cr3 重新赋值可使 TLB 失效
	mov eax, cr3 
	mov cr3, eax

	pop eax

	ret
