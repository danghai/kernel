# PCI Driver

### 1. PCI Background

Peripheral Component Interconnect (PCI) is a standard that describes how to connects the peripheral components
of a system together in a structured and controlled way. The standard describes the way that the system components
are electrically connected and the way that they should behave. Example PCI Based System.

![Example PCI Based System](http://www.tldp.org/LDP/tlk/dd/pci-system.gif)

We discussed [Char device](https://github.com/danghai/Kernel/tree/master/character_device_driver) before and 
already implemented open(), release(), read(), write(). Now we need to implement PCI Subsystem as well as PCI 
bus driver. The block diagram shows the idea:  

![PCI image](https://github.com/danghai/Kernel/tree/master/pci/skeleton_pci/pci.jpg)







### Reference: 

* [Linux PCI driver from Free-Electrons](http://free-electrons.com/doc/legacy/pci-drivers/pci-drivers.pdf)

* [Writing a Linux PCI Device Driver, A Tutorial with a QEMU Virtual Device](http://nairobi-embedded.org/linux_pci_device_driver.html)

* [Basic PCI](http://www.tldp.org/LDP/tlk/tlk.html)

