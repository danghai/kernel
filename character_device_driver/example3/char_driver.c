/*
*   char_driver.c: Device to demonstrate read/write calls of the driver
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

MODULE_AUTHOR ("Hai Dang Hoang");
MODULE_LICENSE ("GPL");
MODULE_DESCRIPTION ("Device to demonstrate read/write calls of the driver");

#define DEVICE_CNT 7
#define DEVICE_NAME "chardev"   /* Dev name as it appears in /proc/devices */
#define SUCCESS 0


/*
*   Prototypes function of character device driver
*/
static int device_open (struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read (struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write (struct file *, const char __user *, size_t, loff_t *);

/*
*   Global variable and parameter
*/
static int Device_Open = 0;	    /* Is device open? Used to prevent multiple access to device */
static dev_t dev; /* dev represents device numbers within the kernel*/
static struct cdev my_cdev; /* Struct cdev represents char devices internally*/
static char buffer[PAGE_SIZE];

/* File operations for character device */
static struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

/*
*  Method open called when a process tries to open the device file, like
*  "cat /dev/mycharfile"
*/
static int device_open(struct inode *inode, struct file *file)
{
    if (Device_Open)
      return -EBUSY;

    Device_Open++;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

/*
* Called when a process closes the device file
*/

static int device_release(struct inode *inode, struct file *file)
{
    Device_Open--;    /* We're now ready for our next caller*/
    /* Decrement the usage count, or else once you opened the file, you'll
     never get rid of the module. */
    module_put(THIS_MODULE);

    pr_info("! Data in Kernel = %s \n",buffer);
    return SUCCESS;
}


/*
* Called when a process, which already opened the dev file, attempts to read from it
*/
static ssize_t device_read (struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    int nbytes;
    int maxbytes;
    int bytes_to_do;

    maxbytes = PAGE_SIZE  - *offp;
    if(maxbytes > count)  /* If maxbytes is greater than required byte to read */
      bytes_to_do = count;
    else
      bytes_to_do = maxbytes;

    nbytes = bytes_to_do - copy_to_user(buff, buffer + *offp, bytes_to_do);
    *offp += nbytes;
    pr_info("! Data send to user [ %s ], nbytes = %d\n",buffer, nbytes);
    return nbytes;
}


/*
* Called when a process, which already opened the dev file, attempts to write from it
*/
static ssize_t device_write (struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
    /* local kernel memory */
    char *kern_buf;
    int nbytes,i;

    if(!buff)
      return -EINVAL;

    /* Allocate memory in Kernel */
    kern_buf = kmalloc (count, GFP_KERNEL);
    if(!kern_buf)
      return -ENOMEM;

    /*
    *   Calculate the number of bytes read
    *   transfer data from user to buffer into kernel
    */
    nbytes = count - copy_from_user(kern_buf, buff, count);
    /* Update the offset */
    *offp += nbytes;

    for(i =0 ;i <count;i++)
      buffer[i]=kern_buf[i];
    /* Display information. pr_info is the same printk with KERN_INFO*/
    pr_info ("! Received data from user [ %s ], nbytes = %d\n",buffer,nbytes);

    return nbytes;
}
/*
*   This function is called when the module is loaded
*/
static int __init char_dev_init (void)
{
   printk (KERN_INFO "Module loading and initial setup character driver ....\n");
   /* dynamically allocate a char driver */
   if(alloc_chrdev_region(&dev,0,DEVICE_CNT, DEVICE_NAME))
   {
      printk(KERN_ERR "alloc_chrdev_region() failed ! \n");
      return -1;
   }

    printk(KERN_INFO "Allocated %d devices at Major: %d \n",DEVICE_CNT, MAJOR(dev));
   /* Initialize the character device and add it to the kernel */
   cdev_init(&my_cdev, &fops);
   my_cdev.owner = THIS_MODULE;

   if(cdev_add(&my_cdev, dev,DEVICE_CNT))
   {
      printk(KERN_ERR "cdev_add() failed ! \n");
      /* clean up chrdev allocation if failed */
      unregister_chrdev_region(dev, DEVICE_CNT);
      return -1;
   }

   memset(buffer, '\0', PAGE_SIZE);
   return 0;
}
/*
*  This function is called when the module is unloaded
*/
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
