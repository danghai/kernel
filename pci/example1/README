# Example PCI Ethernet

A simple PCI device driver. ( The Hardware: 82545EM Gigabit Ethernet) It can read the information about status register: Link, FD, and Speed. In addition, it can report the status LED status on Ethernet port.

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

Make and then insmod the module driver

```
	danghai@ubuntu:~/Kernel/pci/example1$ make
	danghai@ubuntu:~/Kernel/pci/example1$ sudo insmod pci_Ethernet.ko
```

The log file on `dmesg` : 

```
[11602.009075] Module load...
[11602.009497] PCI probe ... 
[11602.009587] Vendor  ID = 0x8086
[11602.009683] Device ID = 0x100f
[11602.009711] Interrupt Line = 7
[11602.010636] PCI probe: BAR0 is defined in IO memory space. 
[11602.011063] BAR0 Address of Register = 0xc00249 
[11602.011345] 
                Intel Corporation 82545EM Gigabit Ethernet Controller: 0x0000cb83 
[11602.011350] Link: UP 
[11602.011353] Speed: 1000-Mbps 
[11602.011355] FD: FULL-DUPLEX 
[11602.011442] Value of LEDCTRL (Offset 0xE00) = 0x7068302 
```

You can see the [datasheet 82545EM ](https://github.com/danghai/Kernel/blob/master/pci/skeleton_pci/8254x_GBe_SDM.pdf). The address of register in BAR0 we access: STATUS = `0x8h` and
LEDCTL = `0xE00h`. 

![status register](https://github.com/danghai/Kernel/blob/master/pci/skeleton_pci/Status_1.JPG)
![status register](https://github.com/danghai/Kernel/blob/master/pci/skeleton_pci/Status_1.JPG)

![LEDCTL](https://github.com/danghai/Kernel/blob/master/pci/skeleton_pci/LEDCTL.JPG)

From datasheet, the default value of LEDCTL = `0x7068302`. You can verify the result on dmesg to make sure we can read value from pci device. 
