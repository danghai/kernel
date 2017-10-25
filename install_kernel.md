# How to build and install the latest Linux Kernel

A guide give you step-by-step how to build and install the latest Linux Kernel.
In this guide, I install the Kernel with version `4.13.9` along with Ubuntu `17.04`.
You need a system with virtualization (VT-d), at least 4GB of RAM, and 60GB of free
hard drive space in order to run Linux in a VM (I got the problem about Kernel Panic out
of memory when I reboot with new Kernel version). You also need to have an 
internet connection to download the source code

### 1. Setup Linux kernel code repository, downloading and extracting the latest kernel source

You can download the latest stable version at [https://www.kernel.org/](https://www.kernel.org/). 
In this time, the latest stable kernel version is `4.13.9`. Download and change to the directory
and extract them: 

```
	tar xf linux-4.13.9.tar.xz
	cd linux-4.13.9
```

Now you need to compile the kernel. The first step is setting up your kernel configuration

### 2. Setting up your kernel configuration and Compiling

You need to duplicate your current config that is stored somewhere 
in /boot/. There might be several files that start with config, so you 
want the one associated with your running kernel. You can find that by running 
`uname -r` and finding the config file that ends with your kernel version number.
Copy that file into the source directory as config by command: 

```
	cp /boot/config-$(uname -r) .config
```

If you need to make any changes to your configuration, run this command to silently update
any new configuration values to their default

```
	 make menuconfig
```

### 3. Building the kernel

You can use following command to `make` and `compile` the file:

```
	make
	sudo make modules_install
	sudo make install
```

However, you can speed up the process if you have a multi-core processor by command `make -jX` Where X is a 
number like 2 or 4. -j option to specify the number of cores to be used. In my case, I use 4 cores. I combine
3 command line to reduce the waiting time.

```
	sudo make -j 4 && sudo make modules_install -j 4 && sudo make install -j 4
```

### 4. Final step to run your kernel

When the kernel is compiled and installed, we need to set the new kernel for the next time boot up:

```
	sudo update-initramfs -c -k 4.13.9
```

And then, you need to tell grub that you made a change, so that can re-write some automatically
generated file. 

```
	sudo update-grub	
``` 

Now, restart the system. You probably see that the new kernel is added to the boot loader entries. Note that if you 
have any problem, you should try the whole thing again!

```
	reboot
```

Note: You will only be using the steps listed under Configuring and Compiling each time something new 
is to be implemented or configured differently.

```
	cp /boot/config-$(uname -r) .config
	make menuconfig
	sudo make -j 4 && sudo make modules_install -j 4 && sudo make install -j 4
```


