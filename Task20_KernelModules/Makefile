obj-m += ./test_fs_module.o
obj-m += ./test_fs_sys_proc_module.o

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 
 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
