CC = gcc
FLAG = -o
obj-m += pci_driver.o
KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(shell pwd) modules
	$(CC) $(FLAG) user user.c
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(shell pwd) clean
	rm user
