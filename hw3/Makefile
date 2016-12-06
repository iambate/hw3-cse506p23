obj-m += var_arg.o
obj-m += tp.o
obj-m += tp2.o
obj-m += module_1.o

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all: ioctl k_module

ioctl: ioctl.c
	gcc -Wall -I$(INC)/generated/uapi -I$(INC)/uapi ioctl.c -o ioctl

k_module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
