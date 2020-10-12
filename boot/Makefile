boot.bin: boot.asm
	nasm boot.asm -o boot.bin -l boot.lst

.PHONY: install clean

# boot.bin 写入扇区 0
install:
	dd if=boot.bin of=../c.img bs=512 count=1 conv=notrunc

clean:
	rm -rf *.bin *.lst
