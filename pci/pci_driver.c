#include <linux/timer.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/pci.h>

MODULE_AUTHOR ("Hai Dang Hoang");
MODULE_LICENSE("GPL");
#define PE_REG_LEDS 0x00E00
#define VENDOR_ID 0x8086
#define DEVICE_ID 0x100f
#define DEVCNT 5
#define DEVNAME "hw4"
static u32 led_on = 0x7844E;

static struct mydev_dev {
    struct cdev cdev;
    dev_t mydev_node;
    int syscall_val;
    void * hw_addr;
} mydev;


struct pes {
        struct pci_dev *pdev;
        void *hw_addr;
};

static char *pe_driver_name = "Realtek_poker";

static DEFINE_PCI_DEVICE_TABLE(pe_pci_tbl) = {
        { PCI_DEVICE(VENDOR_ID, DEVICE_ID) },
        { }, /* must have an empty entry at the end! */
};

static int pe_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
        struct pes *pe;
        u32 ioremap_len;
        int err;

        err = pci_enable_device_mem(pdev);
        if (err)
                return err;

        /* set up for high or low dma */
        err = dma_set_mask(&pdev->dev, DMA_BIT_MASK(64));
        if (err) {
                dev_err(&pdev->dev, "DMA configuration failed: 0x%x\n", err);
                goto err_dma;
        }

        /* set up pci connections */
        err = pci_request_selected_regions(pdev, pci_select_bars(pdev,
                                           IORESOURCE_MEM), pe_driver_name);
        if (err) {
                dev_info(&pdev->dev, "pci_request_selected_regions failed %d\n", err);
                goto err_pci_reg;
        }

        pci_set_master(pdev);

        pe = kzalloc(sizeof(*pe), GFP_KERNEL);
        if (!pe) {
                err = -ENOMEM;
                goto err_pe_alloc;
        }
        pe->pdev = pdev;
        pci_set_drvdata(pdev, pe);

        /* map device memory */
        ioremap_len = min_t(int, pci_resource_len(pdev, 0), 4096); //select BAR0
        pe->hw_addr = ioremap(pci_resource_start(pdev, 0), ioremap_len);
        if (!pe->hw_addr) {
                err = -EIO;
                dev_info(&pdev->dev, "ioremap(0x%04x, 0x%04x) failed: 0x%x\n",
                         (unsigned int)pci_resource_start(pdev, 0),
                         (unsigned int)pci_resource_len(pdev, 0), err);
                goto err_ioremap;
        }

    mydev.hw_addr = pe->hw_addr + PE_REG_LEDS;

        return 0;

err_ioremap:
        kfree(pe);
err_pe_alloc:
        pci_release_selected_regions(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
err_pci_reg:
err_dma:
        pci_disable_device(pdev);
        return err;
}

/* modprobe r8169 */

static void pe_remove(struct pci_dev *pdev)
{
        struct pes *pe = pci_get_drvdata(pdev);

        /* unmap device from memory */
        iounmap(pe->hw_addr);

        /* free any allocated memory */
        kfree(pe);

        pci_release_selected_regions(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
        pci_disable_device(pdev);
}

static struct pci_driver pe_driver = {
        .name     = "pci_example",
        .id_table = pe_pci_tbl,
        .probe    = pe_probe,
        .remove   = pe_remove,
};



static int part3_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "successfully opened!\n");

    printk(KERN_INFO "Ding!\n");
    led_on= led_on^0x40;
    writel((unsigned int) led_on, mydev.hw_addr);



    return 0;
}

static ssize_t part3_read(struct file *file, char __user *buf,
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

    mydev.syscall_val = readl(mydev.hw_addr);

     if (copy_to_user(buf, &mydev.syscall_val, sizeof(unsigned int))) {
        ret = -EFAULT;
        goto out;
    }
    ret = sizeof(int);
    *offset += len;

    /* Good to go, so printk the thingy */
    printk(KERN_INFO "led_reg = 0x%06x \n", mydev.syscall_val);
out:
    return ret;
}

static ssize_t part3_write(struct file *file, const char __user *buf,
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
    .open = part3_open,
    .read = part3_read,
    .write = part3_write,
};

static int __init hw4_init(void)
{
    int ret;



        printk(KERN_INFO "%s loaded\n", pe_driver.name);
        ret = pci_register_driver(&pe_driver);

    printk(KERN_INFO "part2 module loading... \n");

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

static void __exit hw4_exit(void)
{

    pci_unregister_driver(&pe_driver);
    printk(KERN_INFO "%s unloaded\n", pe_driver.name);

    /* destroy the cdev */
    cdev_del(&mydev.cdev);

    /* clean up the devices */
    unregister_chrdev_region(mydev.mydev_node, DEVCNT);

    printk(KERN_INFO "example5 module unloaded!\n");
}

module_init(hw4_init);
module_exit(hw4_exit);
