obj-m += var_arg.o
obj-m += tp.o
obj-m += module_1.o

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
