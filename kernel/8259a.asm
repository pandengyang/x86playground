	global init_8259a
	global reenable_8259a

init_8259a:
	; 020h ICW1 8259A config
	mov al, 011h
	out 020h, al
	call io_delay

	; 0A0h ICW1 8259A config
	out 0A0h, al
	call io_delay

	; 021h ICW2 vector NO.
	mov al, 020h
	out 021h, al
	call io_delay

	; 0A1h ICW2 vector NO.
	mov al, 028h
	out 0A1h, al
	call io_delay

	; 021h ICW3 cascade
	mov al, 04h
	out 021h, al
	call io_delay

	; 0A1h ICW3 cascade
	mov al, 02h
	out 0A1h, al
	call io_delay

	; 021h ICW4 8259a config
	mov al, 001h
	out 021h, al
	call io_delay

	; 0A1h ICW4 8259a config
	out 0A1h, al
	call io_delay

	; 禁用一些中断，只开启时钟中断
	mov al, 11111110b
	out 021h, al
	call io_delay

	mov al, 11111111b
	out 0A1h, al
	call io_delay

	ret

io_delay:
	nop
	nop
	nop
	nop
	ret

reenable_8259a:
	mov al, 020h
	out 020h, al
	ret
