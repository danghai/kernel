# 2nd Example PCI Ethernet
A simple PCI device driver. ( The Hardware: 82545EM Gigabit Ethernet) It can read the information about status register: Link, FD, and Speed. In addition, it can report the status LED status on Ethernet port. Modify the status of LED and then store back value in register. In this case, it will blink the LED on Ethernet port

### How to run

Run command `lspci -v` to know the bus, device, function of PCI_Ethernet and the default
driver

```
02:01.0 Ethernet controller: Intel Corporation 82545EM Gigabit Ethernet Controller (Copper) (rev 01)
	DeviceName: Ethernet0
	Subsystem: VMware PRO/1000 MT Single Port Adapter
	Physical Slot: 33
	Flags: bus master, 66MHz, medium devsel, latency 0, IRQ 19
	Memory at fd5c0000 (64-bit, non-prefetchable) [size=128K]
	Memory at fdff0000 (64-bit, non-prefetchable) [size=64K]
	I/O ports at 2000 [size=64]
	[virtual] Expansion ROM at fd500000 [disabled] [size=64K]
	Capabilities: <access denied>
	Kernel driver in use: e1000
	Kernel modules: e1000
```

First of all, we need to disable the Kernel driver e1000

```
	sudo modprobe -r e1000
```

Make and then insmod the module driver, create the character device driver

```
	danghai@ubuntu:~/Kernel/pci/example1$ make
	danghai@ubuntu:~/Kernel/pci/example1$ sudo insmod pci_Ethernet.ko
	danghai@ubuntu:~/Kernel/pci/example1$ sudo mknod /dev/chardev c 246 0 --> In my machine it is Major: 246. You can view on dmesg)
	danghai@ubuntu:~/Kernel/pci/example1$ cat /dev/chardev		      --> Open the character driver	
```

