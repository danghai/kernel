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
#define USB_MINOR_BASE 0

//static struct usb_device *device;
//static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_SIZE];

/* Structure to hold all of our device specific stuff */
struct usb_pen_driver {
    struct usb_device     *device;               /* the usb device for this device*/
    struct usb_interface  *interface;            /* the interface for this divce */
    unsigned char         *bulk_in_buffer;       /* the buffer to receive data */
    size_t                bulk_in_size;          /* the size of receive buffer */
    __u8                  bulk_in_endpointAddr;  /* the address of the bulk in endpoint*/
    __u8                  bulk_out_endpointAddr; /* the address of the bulk out endpoint*/
};


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
  struct usb_pen_driver *dev;
  dev = f->private_data;
  /* Read the data from the bulk endpoint */
  retval = usb_bulk_msg(dev->device,
                        usb_rcvbulkpipe(dev->device,dev->bulk_in_endpointAddr),
                        bulk_buf,
                        MIN(dev->bulk_in_size,cnt), &read_cnt,5000);

  /* If the read was successful, copy the data to userspace */
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
  struct usb_pen_driver *dev;
  dev = f->private_data;
  if(copy_from_user(bulk_buf,buf, MIN(cnt,dev->bulk_in_size)))
      return -EFAULT;
  retval = usb_bulk_msg(dev->device,
                        usb_sndbulkpipe(dev->device,
                        dev->bulk_out_endpointAddr),
                        bulk_buf,
                        MIN(cnt,dev->bulk_in_size), &wrote_cnt,5000);
  if(retval)
  {
      printk(KERN_ERR "!Write: bulk message returned %d\n", retval);
      return retval;
  }
  return wrote_cnt;
}

static struct file_operations fops =
{
    .owner    = THIS_MODULE,
    .open     = pen_open,
    .release  = pen_close,
    .read     = pen_read,
    .write    = pen_write,
};

/*
 * usb class driver info in order to get a minor number from the usb core
 * and to have the device registered with devfs and the driver core
 */

static struct usb_class_driver usb_class = {
  .name = "usb/pen%d",
  .fops = &fops,
};
static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;
    struct usb_pen_driver *dev;
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    size_t buffer_size;
    int i;

    /* allocate memory for our device state and initialize it */
    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev) {
      dev_err(&interface->dev, "Out of memory\n");
      return 1;
    }
        printk(KERN_INFO "Pen i/f %d driver (%04X:%04X) plugged.\n",iface_desc->desc.bInterfaceNumber,
          id->idVendor, id->idProduct);

    printk(KERN_INFO "ID->bNumEndpoints: %02X\n",
          iface_desc->desc.bNumEndpoints);

    printk(KERN_INFO "ID->bInterfaceClass: %02X\n",
              iface_desc->desc.bInterfaceClass);

    for ( i = 0; i < iface_desc->desc.bNumEndpoints; i++)
    {
          endpoint = &iface_desc->endpoint[i].desc;

          printk(KERN_INFO "ED[%d]->bEndpointAddress: 0x%02X\n",
                       i, endpoint->bEndpointAddress);
          printk(KERN_INFO "ED[%d]->bmAttributes: 0x%02X\n",
                       i, endpoint->bmAttributes);
          printk(KERN_INFO "ED[%d]->wMaxPacketSize: 0x%04X (%d)\n",
                       i, endpoint->wMaxPacketSize,
                       endpoint->wMaxPacketSize);

          buffer_size = endpoint->wMaxPacketSize;
          dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
          dev->bulk_in_size =  buffer_size;
          //dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
    }
    dev->device = interface_to_usbdev(interface);

  //  class.name = "usb/pen%d";
  //  class.fops = &fops;
    if ((retval = usb_register_dev(interface, &usb_class)) < 0)
      printk(KERN_INFO "Not able to get a minor for this device. \n");
    else
      printk(KERN_INFO "Minor obtained: %d \n", interface ->minor);

    return retval;
}

static void pen_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Pen drive removed \n");
    usb_deregister_dev(interface, &usb_class);
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
