obj-m += var_arg.o
obj-m += tp.o
obj-m += tp2.o
obj-m += module_1.o
obj-m += sys_clone2.o

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all: xhw3 clone2 ioctl assign

assign: assign.c
	gcc -Wall -I$(INC)/generated/uapi -I$(INC)/uapi assign.c -o assign

xhw3: xhw3.c
	gcc -Wall -I$(INC)/generated/uapi -I$(INC)/uapi xhw3.c -o xhw3

ioctl: ioctl.c
	gcc -Wall -I$(INC)/generated/uapi -I$(INC)/uapi ioctl.c -o ioctl

clone2:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f xhw3
	rm -f ioctl
