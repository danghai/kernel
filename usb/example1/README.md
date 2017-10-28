# Example USB device driver

A simple usb device driver. The hardware USB is PNY pen driver (ID 154b:00d4 PNY)

### 1. usbview

A basic listing of all detected USB devices can be obtained using the `lsusb` command with and without the pen drive plugged in.
A `-v` option to lsusbprovides detailed information: --> PNY
```
danghai@ubuntu:~$ sudo lsusb
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 002 Device 004: ID 0e0f:0008 VMware, Inc.
Bus 002 Device 003: ID 0e0f:0002 VMware, Inc. Virtual USB Hub
Bus 002 Device 002: ID 0e0f:0003 VMware, Inc. Virtual Mouse
Bus 002 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub

danghai@ubuntu:~$ sudo lsusb
Bus 001 Device 002: ID 154b:00d4 PNY
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 002 Device 004: ID 0e0f:0008 VMware, Inc.
Bus 002 Device 003: ID 0e0f:0002 VMware, Inc. Virtual USB Hub
Bus 002 Device 002: ID 0e0f:0003 VMware, Inc. Virtual Mouse
Bus 002 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub
```
If you use the vmware and do not see the USB hardware connection, you need to set "Show all usb input device" in VM/setting:

![USB](https://github.com/danghai/Kernel/blob/master/usb/example1/usb.JPG)


### 2. How to run

Before insmod the module, we need to deregister the default usb driver that PNY used. `dmesg` show after connecting it:

```
[Oct28 16:00] usb 1-1: new high-speed USB device number 3 using ehci-pci
[  +0.561023] usb 1-1: New USB device found, idVendor=154b, idProduct=00d4
[  +0.000014] usb 1-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[  +0.000007] usb 1-1: Product: USB 2.0 FD
[  +0.000006] usb 1-1: Manufacturer: PNY
[  +0.000005] usb 1-1: SerialNumber: 070174D4AA3D5264
[  +0.075439] usb-storage 1-1:1.0: USB Mass Storage device detected
[  +0.005136] scsi host34: usb-storage 1-1:1.0
[  +0.000199] usbcore: registered new interface driver usb-storage
[  +0.002642] usbcore: registered new interface driver uas
[  +1.704367] scsi 34:0:0:0: Direct-Access     PNY      USB 2.0 FD       PMAP PQ: 0 ANSI: 6
[  +0.004617] sd 34:0:0:0: Attached scsi generic sg2 type 0
[  +0.005480] sd 34:0:0:0: [sdb] 121012224 512-byte logical blocks: (62.0 GB/57.7 GiB)
[  +0.002513] sd 34:0:0:0: [sdb] Write Protect is off
[  +0.000011] sd 34:0:0:0: [sdb] Mode Sense: 23 00 00 00
```

the `idVendor` = 0x154b, `idProduct` = 0x00d4. `usbcore`: uas and usb-storage. You can check by command `lsmod`

deregister them by command:

```
	sudo modprobe -r uas
	sudo modprobe -r usb_storage
```

Make, insmod, and rmmod the module:

```
	make
	sudo insmod usb_driver.ko
	sudo rmmod usb_driver.ko
```

The output should be in dmesg:

```
[  +2.109028] Pen driver (154B:00D4) plugged.
[  +0.001420] usbcore: registered new interface driver pen_driver
[Oct28 16:10] usbcore: deregistering interface driver pen_driver
[  +0.000013] Pen drive removed 	
```
