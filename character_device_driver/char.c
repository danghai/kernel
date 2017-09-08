/*
*   Name: Hai Dang Hoang
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/cdev.h>

MODULE_AUTHOR ("Hai Dang Hoang");
MODULE_LICENSE ("GPL");
/*
*   Prototypes
*/

static int device_open (struct inode *, struct file *);
static ssize_t device_read (struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write (struct file *, const char __user *, size_t, loff_t *);

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

}

static void __exit char_dev_exit(void)
{

}

module_init(char_dev_init);
module_exit(char_dev_exit);
