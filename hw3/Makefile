obj-m += var_arg.o
obj-m += tp.o
obj-m += tp2.o
obj-m += module_1.o
obj-m += ioctl.o

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

ioctl: ioctl.c
	gcc -Wall -I$(INC)/generated/uapi -I$(INC)/uapi ioctl.c -o ioctl

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
