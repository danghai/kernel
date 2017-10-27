# 1. Kernel
 
### 1. [How to build and install the latest Linux Kernel](https://github.com/danghai/Kernel/tree/master/install_kernel.md)

# 2. The Linux Kernel Module Programming 

This repository contains a collection of simple C programs that demonstrate different 
aspects of programming on Linux. The Linux Kernel Module Programming was written for the 4.4 kernels by
Hai Dang. It covers the fundamental and essential linux device driver and Kernel programming. All programs
should compile and run on modern Linux machines. Compile all of them by typing `make` and clean up all files
by typing `make clean`. These topic include: 

### 1. [Hello Kernel](https://github.com/danghai/Kernel/tree/master/hello_kernel)

### 2. [Character Device Driver](https://github.com/danghai/Kernel/tree/master/character_device_driver)  !!!

### 3. [Time Jiffies](https://github.com/danghai/Kernel/tree/master/time_jiffies)

### 4. [Tasklet and Workqueue](https://github.com/danghai/Kernel/tree/master/tasklet_workqueue)

### 5. [Interrupt handler](https://github.com/danghai/Kernel/tree/master/irq_handler)

### 6. [PCI Device Driver](https://github.com/danghai/Kernel/tree/master/pci)   !!!

### 7. [USB Device Driver](https://github.com/danghai/Kernel/tree/master/usb)   !!!

### Downloading 

To download this repository onto a Linux machine that has git installed, try:

```
	git clone https://github.com/danghai/Kernel.git
	cd Kernel
```

To ensure that your copy of the repository is up-to-date, run the following comamnd: 

```
	git pull
```

If you want to ensure that your copy of the repository exactly matches the latest version of the this 
repository, run the following when you are in Kernel directory

```
	git fetch -all
	git reset --hard origin/master
```


### Reference:

* [kernelnewbies.org](https://kernelnewbies.org/)

* [Linux Kernel sphinx](https://kernel.readthedocs.io/en/sphinx-samples/index.html)

* [Linux Kernel Internals (PDF)](http://www.tldp.org/LDP/lki/lki.pdf)

* [The Linux Kernel](http://www.tldp.org/LDP/tlk/tlk.html)

* [Linux Device Drivers ldd3](https://lwn.net/Kernel/LDD3/)

* [Embedded Linux kernel and driver development](http://makelinux.net/books/embedded_linux_kernel_and_drivers/)



