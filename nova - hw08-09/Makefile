c compile :
	make -C user
	make -C kern/build

r run :
	qemu-system-i386 -serial stdio -kernel kern/build/hypervisor -initrd user/hello

cl clean :
	make -C kern/build clean

cla cleanall : clean
	make -C kern/build cleanall

hw10 :
	zip hw10.zip kern/src/ec_syscall.cc

hw11 :
	zip hw11.zip usr/malloc.cc

hw12 :
	zip hw12.zip kern/scr/ec_syscall.cc kern/scr/context_construct.cc kern/include/ec.h
