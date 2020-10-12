kernel.bin: kernel.asm \
	table.c \
	page.c \
	segment.c \
	gdt.c \
	idt.c \
	8259a.asm \
	process.c \
	init.c \
	idle.c \
	hd.asm \
	tlb.asm \
	debug.c \
	libc/stdio.c \
	libc/string.c \
	global.h \
	include/x86playground/type.h \
	include/x86playground/page.h \
	include/x86playground/segment.h \
	include/x86playground/process.h \
	include/x86playground/elf32.h \
	include/x86playground/syscall.h \
	include/stdio.h \
	include/string.h
	nasm -f elf32 -o kernel.o -l kernel.lst kernel.asm
	gcc -c table.c -o table.o -I. -I./include
	gcc -c page.c -o page.o -I. -I./include
	gcc -c segment.c -o segment.o -I. -I./include
	gcc -c gdt.c -o gdt.o -I. -I./include
	gcc -fno-stack-protector -c idt.c -o idt.o -I. -I./include
	nasm -f elf32 -o 8259a.o 8259a.asm
	gcc -c process.c -o process.o -I. -I./include
	nasm -f elf32 -o hd.o hd.asm
	nasm -f elf32 -o tlb.o tlb.asm
	gcc -c init.c -o init.o -I. -I./include
	gcc -c idle.c -o idle.o -I. -I./include
	gcc -Wno-div-by-zero -c debug.c -o debug.o -I. -I./include
	gcc -c libc/stdio.c -o stdio.o -I. -I./include
	gcc -fno-stack-protector -c libc/string.c -o string.o -I. -I./include
	ld -s -Ttext 0xC0400000 -o kernel.bin kernel.o table.o page.o segment.o gdt.o idt.o 8259a.o process.o init.o idle.o hd.o tlb.o debug.o stdio.o string.o

.PHONY: install fmt clean

# .ksize 写入扇区 1
# kernel.bin 写入扇区 2 开始的若干扇区
install:
	dd if=kernel.bin of=../c.img bs=512 seek=2 count=`./ksize.py` conv=notrunc; \
	dd if=.ksize of=../c.img bs=512 seek=1 count=1 conv=notrunc

fmt:
	find . -name "*.c" | xargs -I {} indent -kr -i8 {} -o {}

clean:
	rm -rf .ksize *.bin *.lst *.o
