obj-m += my_scull.o
KERNEL_DIR = /usr/src/linux-headers-`uname -r`

all:
	make -C $(KERNEL_DIR) SUBDIRS=$(shell pwd) modules
	cc -o test_my_scull_drv test_my_scull_drv.c

clean:
	rm -rf *.o *.ko *.mod *~ *.order *.symvers *~ test_my_scull_drv
