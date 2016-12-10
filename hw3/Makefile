obj-m += module_1.o
obj-m += sys_clone2.o
obj-m += module_2.o

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all: xhw3 ioctl assign kernel_mod test_clone

assign: assign.c
	gcc -Wall -I$(INC)/generated/uapi -I$(INC)/uapi assign.c -o assign

xhw3: xhw3.c
	gcc -Wall -I$(INC)/generated/uapi -I$(INC)/uapi xhw3.c -o xhw3

test_clone: test_clone.c test2_clone.c
	gcc -Wall -I$(INC)/generated/uapi -I$(INC)/uapi test_clone.c -o test_clone
	gcc -Wall -I$(INC)/generated/uapi -I$(INC)/uapi test2_clone.c -o test2_clone


ioctl: ioctl.c
	gcc -Wall -I$(INC)/generated/uapi -I$(INC)/uapi ioctl.c -o ioctl

kernel_mod:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f xhw3
	rm -f ioctl
	rm -f assign
	rm -f test_clone
	rm -f test2_clone
