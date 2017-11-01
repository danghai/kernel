/*
*   usb_driver.c: A simple USB Pen registration Driver
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define MIN(a,b) (((a) <= (b))? (a): (b))
#define BULK_OUT 0x02
#define BULK_IN 0x81
#define MAX_SIZE 512
#define VENDOR_ID 0x154b
#define PRODUCT_ID 0x00d4

static struct usb_device *device;
static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_SIZE];

static int pen_open(struct inode *i, struct file *f)
{
  return 0;
}

static int pen_close(struct inode *i, struct file *f)
{
  return 0;
}

static ssize_t pen_read(struct file *f, char __user *buf, size_t cnt, loff_t *off)
{
  int retval;
  int read_cnt;

  /* Read the data from the bulk endpoint */
  retval = usb_bulk_msg(device, usb_rcvbulkpipe(device,BULK_IN),
                        bulk_buf, MAX_SIZE, &read_cnt,5000);
  if (retval)
  {
      printk(KERN_ERR "!Read: Bulk message returned %d\n", retval);
      return retval;
  }
  if (copy_to_user(buf,bulk_buf, MIN(cnt,read_cnt)))
      return -EFAULT;

  return MIN(cnt,read_cnt);
}

static ssize_t pen_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off)
{
  int retval;
  int wrote_cnt = MIN (cnt, MAX_SIZE);

  if(copy_from_user(bulk_buf,buf, MIN(cnt,MAX_SIZE)))
      return -EFAULT;
  retval = usb_bulk_msg(device,usb_sndbulkpipe(device, BULK_OUT),
                        bulk_buf, MIN(cnt,MAX_SIZE), &wrote_cnt,5000);
  if(retval)
  {
      printk(KERN_ERR "!Write: bulk message returned %d\n", retval);
      return retval;
  }
  return wrote_cnt;
}

static struct file_operations fops =
{
    .open = pen_open,
    .release = pen_close,
    .read = pen_read,
    .write = pen_write,
};



static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;
    printk(KERN_INFO "Pen driver (%04X:%04X) plugged.\n", id->idVendor, id->idProduct);
    device = interface_to_usbdev(interface);

    class.name = "usb/pen%d";
    class.fops = &fops;
    if ((retval = usb_register_dev(interface, &class)) < 0)
      printk(KERN_INFO "Not able to get a minor for this device. \n");
    else
      printk(KERN_INFO "Minor obtained: %d \n", interface ->minor);

    return retval;
}

static void pen_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Pen drive removed \n");
    usb_deregister_dev(interface, &class);
}

static struct usb_device_id pen_table [] =
{
  {USB_DEVICE(VENDOR_ID,PRODUCT_ID)},
  {}
};

MODULE_DEVICE_TABLE (usb, pen_table);
static struct usb_driver pen_driver =
{
    .name = "pen_driver",
    .id_table = pen_table,
    .probe = pen_probe,
    .disconnect = pen_disconnect,
};

static int __init pen_init(void)
{
    return usb_register(&pen_driver);
}

static void __exit pen_exit(void)
{
    usb_deregister(&pen_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hai Dang Hoang");
MODULE_DESCRIPTION("USB Pen Registration Driver");
module_init(pen_init);
module_exit(pen_exit);
