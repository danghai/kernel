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
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_AUTHOR("Hai Dang Hoang");
MODULE_LICENSE("GPL");
#define DEVICE_NAME "pci_test"
#define VENDOR_ID 0x8086        /* Intel */
#define DEVICE_ID 0x100F        /* Ethernet Controller */
#define REG_STATUS 0x0008       /* Status register */
#define ADDR_REG_LEDS 0x00E00   /* Address LED_CTRL */

/* For Character device */
#define DEVCNT 5
#define DEVNAME "chardev"

/* Assuming that our device's configuration header requests 128 bytes on BAR0*/
#define CONFIGURATION_HEADER_REQUEST 128
#define BAR_IO 0
#define SUCCESS 0


/* Constant varibale value for status register */
char *link[] = {"DOWN", "UP"};
char *speed[] = {"10-Mbps", "100-Mbps", "1000-Mbps", "1000-Mbps"};
char  *duplex[] = {"HALF-DUPLEX", "FULL-DUPLEX"};

static int led_on = 0x0E;
static int blink_rate =2;
module_param(blink_rate,int, S_IRUSR | S_IWUSR);
static struct timer_list blink_timer;
/*
* This table holds the list of VendorID, DeviceID of PCI driver
*/
static struct pci_device_id ids[] = {
  { PCI_DEVICE(VENDOR_ID, DEVICE_ID),},
  { 0,  }
};

static struct mydev_dev {
    struct cdev cdev;
    dev_t mydev_node;
    int syscall_val;
} mydev;

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

/*
* timer
*/
static void timer_cb(unsigned long data)
{

  /* blink */
  iowrite32(led_on,&privdata->regs[0]+ ADDR_REG_LEDS);
  printk(KERN_INFO "Ding! Value of LEDCTRL (Offset 0xE00) = 0x%04x \n",ioread32(&privdata->regs[0]+ ADDR_REG_LEDS));
  led_on = led_on ^ 0x40;

  mod_timer(&blink_timer, blink_rate*HZ + jiffies);
}

static int char_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "successfully opened!\n");
    mod_timer(&blink_timer, blink_rate*HZ + jiffies);

    return 0;
}

static ssize_t char_read(struct file *file, char __user *buf,
                             size_t len, loff_t *offset)
{
    /* Get a local kernel buffer set aside */
    int ret;

    if (*offset >= sizeof(int))
        return 0;

    /* Make sure our user wasn't bad... */
    if (!buf) {
        ret = -EINVAL;
        goto out;
    }

     if (copy_to_user(buf, &mydev.syscall_val, sizeof(unsigned int))) {
        ret = -EFAULT;
        goto out;
    }
    ret = sizeof(int);
    *offset += len;

    /* Good to go, so printk the thingy */
    //printk(KERN_INFO "led_reg = 0x%06x \n", mydev.syscall_val);
out:
    return ret;
}

static ssize_t char_write(struct file *file, const char __user *buf,
                              size_t len, loff_t *offset)
{
    /* Have local kernel memory ready */
    char *kern_buf;
    int ret;
//  int temp;

    /* Make sure our user isn't bad... */
    if (!buf) {
        ret = -EINVAL;
        goto out;
    }

    /* Get some memory to copy into... */
    kern_buf = kmalloc(len, GFP_KERNEL);

    /* ...and make sure it's good to go */
    if (!kern_buf) {
        ret = -ENOMEM;
        goto out;
    }

    /* Copy from the user-provided buffer */
    if (copy_from_user(kern_buf, buf, len)) {
        /* uh-oh... */
        ret = -EFAULT;
        goto mem_out;
    }

    kstrtoint(kern_buf, 16, &mydev.syscall_val);

//  writel((unsigned int) mydev.syscall_val,mydev.hw_addr);

    ret = len;

    /* print what userspace gave us */
    printk(KERN_INFO "Userspace wrote \"%s\" to us\n", kern_buf);

mem_out:
    kfree(kern_buf);
out:
    return ret;
}

/* File operations for our device */
static struct file_operations mydev_fops = {
    .owner = THIS_MODULE,
    .open = char_open,
    .read = char_read,
    .write = char_write,
};

static int __init pcidevice_init(void)
{
    int ret;
    printk(KERN_INFO"Module load...\n");
    setup_timer(&blink_timer, timer_cb, (unsigned long)&led_on);
    ret = pci_register_driver(&pcidevice_driver);
    if (alloc_chrdev_region(&mydev.mydev_node, 0, DEVCNT, DEVNAME)) {
        printk(KERN_ERR "alloc_chrdev_region() failed!\n");
        return -1;
    }

    printk(KERN_INFO "Allocated %d devices at major: %d\n", DEVCNT,
           MAJOR(mydev.mydev_node));

    /* Initialize the character device and add it to the kernel */
    cdev_init(&mydev.cdev, &mydev_fops);
    mydev.cdev.owner = THIS_MODULE;

    if (cdev_add(&mydev.cdev, mydev.mydev_node, DEVCNT)) {
        printk(KERN_ERR "cdev_add() failed!\n");
        /* clean up chrdev allocation */
        unregister_chrdev_region(mydev.mydev_node, DEVCNT);

        return -1;
    }

    return ret;
}

static void __exit pcidevice_exit(void)
{
    printk(KERN_INFO "Module unload...\n");
    del_timer(&blink_timer);
    pci_unregister_driver(&pcidevice_driver);
    /* destroy the cdev */
   cdev_del(&mydev.cdev);

   /* clean up the devices */
   unregister_chrdev_region(mydev.mydev_node, DEVCNT);
}

module_init(pcidevice_init);
module_exit(pcidevice_exit);
