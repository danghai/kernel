/*
*   chardev.c: Creates a read-only char device that says how many times
*   you've read from the dev file.
*   Reference: http://tldp.org/LDP/lkmpg/2.6/html/index.html
*   Modify: Hai Dang Hoang (Email: goldsea5191@gmail.com)
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_AUTHOR ("Hai Dang Hoang");
MODULE_LICENSE ("GPL");

/*
*   Prototypes function of character device driver
*/
static int device_open (struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read (struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write (struct file *, const char __user *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"	  /* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80		          /* Max length of the message from the device */

/*
 * Global variables are declared as static, so are global within the file.
 */

static int Major;		            /* Major number assigned to our device driver */
static int Device_Open = 0;	    /* Is device open? Used to prevent multiple access to device */
static char msg[BUF_LEN];	      /* The msg the device will give when asked */
static char *msg_Ptr;

static struct file_operations fops = {
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
    static int counter = 0;
    if(Device_Open)
        return -EBUSY;
}

/*
*   This function is called when the module is loaded
*/
static int __init char_dev_init(void)
{
    printk(KERN_INFO "Module loading and initial setup character driver .... \n");
    Major = register_chrdev(0, DEVICE_NAME, *fops);
    if(Major < 0)
    {
      prink (KERN_ALERT "Registering char device failed with %d \n", Major);
      return Major;
    }

    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with \n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n",DEVICE_NAME, Major);
    printk(KERN_INFO "Try various minor numbers. Ty to cat and echo to \n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done .\n");

    return SUCCESS;
}

/*
* This function is called when the module is unloaded
*/
static void __exit char_dev_exit(void)
{
    /* Unregister the device */
    int ret = unregister_chrdev (Major, DEVICE_NAME);
    if(ret < 0)
        printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);
    printk(KERN_INFO "Module unloaded! ...\n");
}

module_init(char_dev_init);
module_exit(char_dev_exit);
