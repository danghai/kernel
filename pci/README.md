# PCI Driver

### 1. PCI Background

Peripheral Component Interconnect (PCI) is a standard that describes how to connects the peripheral components
of a system together in a structured and controlled way. The standard describes the way that the system components
are electrically connected and the way that they should behave. Example PCI Based System.

![Example PCI Based System](http://www.tldp.org/LDP/tlk/dd/pci-system.gif)

We discussed [Char device](https://github.com/danghai/Kernel/tree/master/character_device_driver) before and 
already implemented open(), release(), read(), write(). Now we need to implement PCI Subsystem as well as PCI 
bus driver. The block diagram shows the idea:  

![PCI image](https://github.com/danghai/Kernel/blob/master/pci/skeleton_pci/pci.JPG)

PCI devices are addressed using bus, device and function numbers: 

```
danghai@ubuntu:~/Kernel/pci$ lspci
00:00.0 Host bridge: Intel Corporation 440BX/ZX/DX - 82443BX/ZX/DX Host bridge (rev 01)
00:01.0 PCI bridge: Intel Corporation 440BX/ZX/DX - 82443BX/ZX/DX AGP bridge (rev 01)
00:07.0 ISA bridge: Intel Corporation 82371AB/EB/MB PIIX4 ISA (rev 08)
00:07.1 IDE interface: Intel Corporation 82371AB/EB/MB PIIX4 IDE (rev 01)
00:07.3 Bridge: Intel Corporation 82371AB/EB/MB PIIX4 ACPI (rev 08)

```

Ex: 00:07.3 --> `00`: PCI bus number, `07`: PCI device number , `3`: Function number

`lspci -v -m -n -s "Address pci"` :PCI devices are identified via `VendorIDs`, `DeviceIDs` and `Class Codes`

`lspci -x` : display the device configuration 

Each PCI device has a 256 byte address space containing configuration registers: Ex

```
02:01.0 Ethernet controller: Intel Corporation 82545EM Gigabit Ethernet Controller (Copper) (rev 01)
00: 86 80 0f 10 17 01 30 02 01 00 00 02 10 00 00 00
10: 04 00 5c fd 00 00 00 00 04 00 ff fd 00 00 00 00
20: 01 20 00 00 00 00 00 00 00 00 00 00 ad 15 50 07
30: 00 00 00 00 dc 00 00 00 00 00 00 00 07 01 ff 00
```

* `Offset 0`: Vendor Id. This 16-bit register identifies a hardware manufacturer. Ex: Vendor number
Intel Device : 0x8086

* `Offset 2`: Device Id. 16 bit selected by the manufacturer. This ID is usually paired with vendor ID
to make a unique 32-bit identifier for a hardware device

* `Offset 10`: Class Id (network, display, bridge ...)

* `Offsets 16 to 39`: Base Address Registers (BAR) 0 to 5

* `Offset 44`: Subvendor Id

* `Offset 46`: Subdevice Id

* `Offset 64 and up`: up to the device manufacture

### 2. Module Device Table

A PCI driver can tell the kernel what kind of devices it supports. The struct `pci_device_id` is
used to define a list of the different types of PCI device that a driver supports. Ex: 

```c

static struct pci_device_id ids[] = {
  {PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL),},
  {0, }
};
```

This `pci_device_id` structure needs to be exported to user space to allow the hotplug and module
loading systems know what module works with what hardware devices. The macro `MODULE_DEVICE_TABLE` 
accomplishes this. Ex:

```c
	MODULE_DEVICE_TABLE(pci,ids);
```

### 3. Registering a PCI Driver

The main structure that all PCI driver must create in order to be registered with the kernel properly 
is the struct `pci_driver` structure. This structure consists of a number of function callbacks
and variables that describe the PCI driver to the PCI core. Ex:

```c
	static struct pci_driver pci_driver = {
  	.name = "pci_skel",
  	.id_table = ids,
  	.probe = probe,
  	.remove = remove,
	};
```

To register the struct pci_driver with the PCI core, a call to `pci_register_driver` is made with a pointer
to the struct pci_driver. This is traditionally done in the module __init:

```c
	pci_register_driver(&pci_driver);
```

When the PCI driver to be unload, the struct pci_driver needs to be unregistered from the kernel.
This is done with a call to `pci_unregister_driver`. When this call happen, any PCI devices that
were currently bound to this driver are removed. 

```c
	pci_unregister_driver(&pci_driver);
```

You can view the [C code Skeleton PCI](https://github.com/danghai/Kernel/blob/master/pci/skeleton_pci/ske_pci.c)

### Example 

Let's look at some of example in practice PCI. Listing examples provide a simple kernel module
that demonstrates the core aspects of the simple PCI

| # | Title | Desciption |
| --- | --- | --- |
| 1 | [Skeleton PCI](https://github.com/danghai/Kernel/blob/master/pci/skeleton_pci/ske_pci.c) | Skeleton PCI device driver|
| 2 | [Example1](https://github.com/danghai/Kernel/tree/master/pci/example1) | Simple PCI Ethernet driver, It can read information register the hardware 82545EM Gigabit Ethernet|
| 3 | [Example2](https://github.com/danghai/Kernel/tree/master/pci/example2) | It can read the information, modify and then store back value. Ex: Blink the LED of Ethernet|

### Reference: 

* [Linux PCI driver from Free-Electrons](http://free-electrons.com/doc/legacy/pci-drivers/pci-drivers.pdf)

* [Writing a Linux PCI Device Driver, A Tutorial with a QEMU Virtual Device](http://nairobi-embedded.org/linux_pci_device_driver.html)

* [Basic PCI](http://www.tldp.org/LDP/tlk/tlk.html)

* [Overview of Peripheral Buses](http://www.xml.com/ldd/chapter/book/ch15.html)


