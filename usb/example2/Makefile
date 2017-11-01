CC = gcc
FLAG = -o
obj-m = usb_driver.o

Kernel_Version = $(shell uname -r)

all:
	make -C /lib/modules/$(Kernel_Version)/build M=$(shell pwd) modules
	$(CC) $(FLAG) user user.c
clean:
	make -C /lib/modules/$(Kernel_Version)/build M=$(shell pwd) clean
	rm user
