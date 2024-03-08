###
### kernel
###
kernel:
	gcc  -m16 -c src/kernel/c/kernel.c -o build/kernel.o -fno-stack-protector # compile the kernel c file to kernel.o
	ld -Ttext 0x1000 -o build/kernel.elf build/kernel.o -melf_i386 -e main # link the o file to elf 
	objcopy -O binary build/kernel.elf build/kernel.bin # convert elf file to kernel.bin

###
### bootloader
###

bootloader:
	nasm -f bin src/bootloader/asm/bootloader.asm -o build/bootloader.bin # compile the bootloader

###
### os
###

os: clean bootloader kernel 
	dd if=/dev/zero of=build/os.img bs=512 count=2880  > /dev/null 2>&1 # make the floppy
	mkfs.fat -F 12 -n "CLESOS" build/os.img # make it use fat12
	dd if=build/bootloader.bin of=build/os.img conv=notrunc > /dev/null 2>&1 # first sector the bootloader
	truncate --size 512 build/kernel.bin # change size of kernel into 512 bytes
	mcopy -i build/os.img build/kernel.bin ::kernel.bin # copy the kernel into the floppy
	mattrib +r +h -i build/os.img ::/kernel.bin # chang it to  hidden
	mmd -i build/os.img new_dir # create folder in the floppy for testing
	mcopy -i build/os.img build/test.txt ::/new_dir/test.txt # put the test.txt int the folder for testing
	
###
### clean
###

clean:
	rm -f build/*.bin # remove the bin files only to don`t remove the test.txt
	rm -f build/*.elf # remove the elf files only to don`t remove the test.txt
	rm -f build/*.o # remove the o files only to don`t remove the test.txt
	rm -f build/*.img # remove the img files only to don`t remove the test.txt
