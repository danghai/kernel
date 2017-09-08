/*
*   Name: Hai Dang Hoang
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
static int data;   /* data pass as a parameter for syscal_val */
module_param(data,int, S_IRUSR | S_IWUSR);



/* File operations for character device */
static struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = device_read,
  .write = device_write,
  .open = device_open
};

/* Define the method open of fops*/
static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Successfully opened! \n");
    printk(KERN_INFO "Currently, the value of data: %d\n",data);
    return 0;
}

/* Define the method read of fops
  + filp : file pointer
  + count: the size of the requested data transfer
  + buff: points to the user buffer holding the data to be written or the empty buffer
  where the newly read data should be placed
  + offp: represent the current file position after successful completion of the system call*/
static ssize_t device_read (struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    int ret;
    if (*offp >= sizeof(int))
      return 0;

    if(!buff)                /* No buffer in user*/
      return -EINVAL;

   /* Transfer data from Kernel to user */
    if (copy_to_user(buff,&data,sizeof(int)))
      return -EFAULT;
    ret = sizeof(int);
    *offp += count;

    /* Confirm that user got exact data */
    printk (KERN_INFO "User got from us: %d \n",data);

    return ret;
}

/* Define the method write of fops
  + filp : file pointer
  + count: the size of the requested data transfer
  + buff: points to the user buffer holding the data to be written or the empty buffer
  where the newly read data should be placed
  + offp: represent the current file position after successful completion of the system call*/
static ssize_t device_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
    /* local kernel memory*/
    char *kern_buf;
    int ret;

    if(!buff)                /* No buffer in user*/
      return -EINVAL;

    /* Allocate memory in kernel */
    kern_buf = kmalloc (count, GFP_KERNEL);
    if (!kern_buf)
      return -ENOMEM;

    /* Transfer data from user to kernel through kernel buffer*/
    if(copy_from_user(kern_buf,buff,count))
    {
        kfree(kern_buf);
        return -EFAULT;
    }

    /* Update global data in kernel */
    if(kstrtoint(kern_buf,10,&data))
    {
        kfree(kern_buf);
        return -ERANGE;
    }
    ret = count;
    /* Print result what userspace gave back the new value by system call */
    printk(KERN_INFO "Good! Userspace wrote back the new data: %d to kernel \n",data);

    return ret;
}





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
