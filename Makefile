OSIMAGE = panes-os.bin

kernel:
	# Assembling bootloader...
	@i586-elf-as boot.s -o boot.o
	# Compiling kernel...
	@i586-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	# Linking kernel and bootloader...
	@i586-elf-gcc -T linker.ld -o ${OSIMAGE} -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

qemu: kernel
	qemu-system-i386 -kernel ${OSIMAGE}

clean:
	@-rm -f *.o 
	@-rm -f ${OSIMAGE}
