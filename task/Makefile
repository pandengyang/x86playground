all: init.c idle.c init.asm idle.asm stdio.c stdio.asm
	# gcc -c stdio.c -o stdio.o -I ../kernel/include
	nasm -f elf32 -o stdio.o stdio.asm
	gcc -c init.c -o init_c.o -I ../kernel/include
	nasm -f elf32 -o init_asm.o init.asm
	ld -s -Ttext 0x08048000 -o init init_c.o init_asm.o stdio.o
	gcc -c idle.c -o idle_c.o -I ../kernel/include
	nasm -f elf32 -o idle_asm.o idle.asm
	ld -s -Ttext 0x08048000 -o idle idle_c.o idle_asm.o stdio.o

.PHONY: install fmt clean

# init 写入扇区 100 开始的 16 个扇区
# idle 写入扇区 116 开始的 16 个扇区
install:
	dd if=init of=../c.img bs=512 seek=100 count=16 conv=notrunc; \
	dd if=idle of=../c.img bs=512 seek=116 count=16 conv=notrunc

fmt:
	find . -name "*.c" | xargs -I {} indent -kr -i8 {} -o {}

clean:
	rm -rf init idle *.o
