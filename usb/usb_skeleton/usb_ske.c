/*
*   USB skeleton
*/
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/usb.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hai Dang Hoang");

struct usb_driver{
  /* One structure for each connected device */
};

static struct usb_device_id driver_table [] = {
  { USB_DEVICE(VENDOR_ID, PRODUCT_ID)},
  { }
};

static int driver_open(struct inode *inode, struct file *file)
{
  /* open syscall */
}

static int driver_release(struct inode *inode, struct file *file)
{
  /* close syscall */
}

static ssize_t driver_write(struct file *file, const char __user *user_buf, size_t
        count, loff_t *ppos);
{
    /* write syscall */
}

static struct file_operations driver_fops = {
    .owner =    THIS_MODULE,
    .write =    driver_write,
    .open =     driver_open,
    .release =  driver_release,
};

static int ske_usb_probe(struct usb_interface *interface, const struct usb_device_id*id)
{
    /* called when a USB device is connected to the computer. */
}

static void ske_usb_disconnect(struct usb_interface *interface)
{
    /* called when unplugging a USB device. */
}

static struct usb_driver ske_usb_driver = {
    .name = "Skeleton USB driver",
    .id_table = driver_table,
    .probe = ske_usb_probe,
    .disconnect = ske_usb_disconnect,
};

static int __init usb_skel_init(void)
{
    /* called on module loading */
}

static void __exit usb_skel_exit(void)
{
    /* called on module unloading */
}
module_init (usb_skel_init);
module_exit (usb_skel_exit);
