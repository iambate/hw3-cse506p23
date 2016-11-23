obj-m += var_arg.o

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all: new_arg

new_arg:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
