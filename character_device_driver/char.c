/*
*   Name: Hai Dang Hoang
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/types.h>
#define DEVICE_CNT 7
#define DEVICE_NAME "chardev"   /* Dev name as it appears in /proc/devices */

MODULE_AUTHOR ("Hai Dang Hoang");
MODULE_LICENSE ("GPL");
/*
*   Prototypes function of character device driver
*/
static int device_open (struct inode *, struct file *);
static ssize_t device_read (struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write (struct file *, const char __user *, size_t, loff_t *);

/*
*   Global variable and parameter
*/

static dev_t dev; /* dev represents device numbers within the kernel*/
static struct cdev my_cdev; /* Struct cdev represents char devices internally*/
static int data   /* data pass as a parameter for syscal_val */
module_param(data,int, S_IRUSR | S_IWUSR);



/* File operations for character device */
static struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

static int __init char_dev_init (void)
{
    printk(KERN_INFO "Module loading and initial setup character driver .... \n");
    /* Dynamically allocate a char driver */
    if(alloc_chrdev_region(&dev,0,DEVICE_CNT,DEVICE_NAME))
    {
        printk(KERN_ERR "alloc_chrdev_region() failed ! \n");
        return -1;
    }

    printk(KERN_INFO "Allocated %d devices at Major: %d \n",DEVICE_CNT, MAJOR(dev));
    /* Initialize the character device and add it to the kernel */
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    if(cdev_add(&my_cdev,dev,DEVICE_CNT))
    {
        printk(KERN_ERR "cdev_add() failed ! \n");
        /* clean up chrdev allocation if failed */
        unregister_chrdev_region(dev, DEVICE_CNT);
        return -1;
    }

    return 0;
}

static void __exit char_dev_exit(void)
{
    /* delete the cdev */
    cdev_del(&my_cdev);

    /* clean up the devices */
    unregister_chrdev_region(dev, DEVICE_CNT);

    printk(KERN_INFO "Module unloaded! ...\n");
}

module_init(char_dev_init);
module_exit(char_dev_exit);
