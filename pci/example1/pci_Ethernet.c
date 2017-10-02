/*
*   pci_Ethernet.c: A simple PCI device driver. ( The Hardware: 82545EM Gigabit Ethernet)
*   It can read the information about status register: Link, FD, and Speed.
*   In addition, it can report the status LED status on Ethernet port.
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/cdev.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <asm/io.h>

MODULE_AUTHOR("Hai Dang Hoang");
MODULE_LICENSE("GPL");
#define DEVICE_NAME "pci_test"
#define VENDOR_ID 0x8086        /* Intel */
#define DEVICE_ID 0x100F        /* Ethernet Controller */
#define REG_STATUS 0x0008       /* Status register */
#define ADDR_REG_LEDS 0x00E00   /* Address LED_CTRL */

/* Assuming that our device's configuration header requests 128 bytes on BAR0*/
#define CONFIGURATION_HEADER_REQUEST 128
#define BAR_IO 0
#define SUCCESS 0

/* Constant varibale value for status register */
char *link[] = {"DOWN", "UP"};
char *speed[] = {"10-Mbps", "100-Mbps", "1000-Mbps", "1000-Mbps"};
char  *duplex[] = {"HALF-DUPLEX", "FULL-DUPLEX"};
/*
* This table holds the list of VendorID, DeviceID of PCI driver
*/

static struct pci_device_id ids[] = {
  { PCI_DEVICE(VENDOR_ID, DEVICE_ID),},
  { 0,  }
};

MODULE_DEVICE_TABLE(pci,ids);

/* Struct store the private data between function */
struct pcidevice_privdata
{
    u16 VendorID, DeviceID;
    u8 InterruptLine;
    void __iomem* regs;
};

static struct pcidevice_privdata *privdata;

/* Probe function*/
static int pcidevice_probe (struct pci_dev *pdev, const struct pci_device_id *id)
{
    printk (KERN_INFO "PCI probe ... \n");
    u16 VendorID, DeviceID;
    u8 InterruptLine;
    int i,rc=0;
    unsigned int status;

    privdata = kzalloc(sizeof(*privdata), GFP_KERNEL);
    if(!privdata)
    {
        printk(KERN_INFO "PCI probe: Failed to allocated memory \n");
        return -ENOMEM;
    }

    /* Set private driver data pointer for a pci_dev*/
    pci_set_drvdata(pdev,privdata);

    /*
    Read two, or four bytes from the configuration space of the device identified by dev.
    The where argument is the byte offset from the beginning of the configuration space.
    The value fetched from the configuration space is returned through ptr,
    and the return value of the functions is an error code.
    */
    pci_read_config_word(pdev, PCI_VENDOR_ID, &VendorID);
    printk(KERN_INFO "Vendor  ID = 0x%x\n", VendorID);

    pci_read_config_word(pdev,PCI_DEVICE_ID, &DeviceID);
    printk(KERN_INFO "Device ID = 0x%x\n",DeviceID);

    pci_read_config_byte(pdev,PCI_INTERRUPT_LINE, &InterruptLine);
    printk(KERN_INFO "Interrupt Line = %d\n", InterruptLine);

    privdata->VendorID = VendorID;
    privdata->DeviceID = DeviceID;
    privdata->InterruptLine = InterruptLine;

    /* Enable the device */
    rc = pci_enable_device(pdev);

    if(rc)
    {
        printk(KERN_INFO "PCI probe: pci_enable_device failed. \n");
        return -ENODEV;
    }

    /* Check the BAR0 is defined and memory mapped
      This function returns the flags associated with this resource.*/
    if((pci_resource_flags(pdev, BAR_IO) & IORESOURCE_MEM) != IORESOURCE_MEM)
    {
        printk(KERN_INFO "PCI probe: BAR0 is not defined in memory space. \n");

        /* Check the BAR0 is defined and IO mapped */
        if((pci_resource_flags(pdev, BAR_IO)& IORESOURCE_IO) != IORESOURCE_IO)
        {
            printk(KERN_INFO "PCI probe: BAR0 is not defined in IO space. \n");
            rc = 1;
        }
        else
            printk(KERN_INFO "PCI probe: BAR0 is defined in IO space. \n");
    }
    else
      printk(KERN_INFO "PCI probe: BAR0 is defined in IO memory space. \n");

    if(rc)
    {
        printk(KERN_INFO "PCI probe: BAR0 is not defined in Memory or IO space.\n");
        return -ENODEV;
    }


  /*Total 6 BARS (regions) could be memory mapped or port-mapped.
    Reserve pci I/O and memory resource
  */
  rc = pci_request_region( pdev, BAR_IO, DEVICE_NAME);
  if(rc)
  {
      printk(KERN_INFO"PCI probe:  BAR0 is not defined in Memory or IO space.\n");
      return -ENODEV;
  }

  /* Using this function you will get a __iomem address to your device BAR.
  You can access it using ioread*() and iowrite*(). */

 privdata->regs = pci_iomap(pdev,BAR_IO,CONFIGURATION_HEADER_REQUEST);

 if (!privdata->regs)
 {
      printk(KERN_INFO "PCI probe: Failed to map BAR 0.\n");
      return -ENODEV;
 }

 printk(KERN_INFO "BAR0 Address of Register = 0x%04x \n",ioread32(&privdata->regs[0]));

/* Enable bus mastering for the device */
pci_set_master(pdev);

/* Dislay the information */
status = ioread32(&privdata->regs[0]+ REG_STATUS);
printk(KERN_INFO "\n Intel Corporation 82545EM Gigabit Ethernet Controller: 0x%08x \n",status);
printk(KERN_INFO "Link: %s \n",link[(status >> 1)&1]);
printk(KERN_INFO "Speed: %s \n",speed[(status >> 6)&3]);
printk(KERN_INFO "FD: %s \n", duplex[(status >> 0)&1]);
printk(KERN_INFO "Value of LEDCTRL (Offset 0xE00) = 0x%04x \n",ioread32(&privdata->regs[0]+ ADDR_REG_LEDS));
return SUCCESS;
}

static void pcidevice_remove(struct pci_dev* pdev)
{
    //Release the IO region
    printk(KERN_INFO"Module remove.\n");
    pci_release_region(pdev, BAR_IO);
    pci_clear_master(pdev);
    pci_disable_device(pdev);
    kfree(privdata);
}

static struct pci_driver pcidevice_driver =
{
    .name = DEVICE_NAME,
    .id_table = ids,
    .probe = pcidevice_probe,
    .remove = pcidevice_remove,
};

static int __init pcidevice_init(void)
{

    printk(KERN_INFO"Module load...\n");
    return pci_register_driver(&pcidevice_driver);
}

static void __exit pcidevice_exit(void)
{
    printk(KERN_INFO "Module unload...\n");
    pci_unregister_driver(&pcidevice_driver);
}

module_init(pcidevice_init);
module_exit(pcidevice_exit);
