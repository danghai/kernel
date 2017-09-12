# Character Device Files

| # | Title | Files |
| --- | --- | --- |
| 1 | [Example1](https://github.com/danghai/Kernel/tree/master/character_device_driver/example1) | [chardev.c](https://github.com/danghai/Kernel/blob/master/character_device_driver/example1/chardev.c)<br>[Makefile](https://github.com/danghai/Kernel/blob/master/character_device_driver/example1/Makefile)<br>[README](https://github.com/danghai/Kernel/blob/master/character_device_driver/example1/README.md)|
| 2 | [Example2](https://github.com/danghai/Kernel/tree/master/character_device_driver/example2)| [char.c](https://github.com/danghai/Kernel/blob/master/character_device_driver/example2/char.c)<br>[user.c](https://github.com/danghai/Kernel/blob/master/character_device_driver/example2/user.c)<br>[Makefile](https://github.com/danghai/Kernel/blob/master/character_device_driver/example2/Makefile)<br>[README](https://github.com/danghai/Kernel/blob/master/character_device_driver/example2/README.md)|
| 3 | [Example3](https://github.com/danghai/Kernel/tree/master/character_device_driver/example3) | [char_driver.c](https://github.com/danghai/Kernel/blob/master/character_device_driver/example3/char_driver.c)<br>[Makefile](https://github.com/danghai/Kernel/blob/master/character_device_driver/example3/Makefile)<br>[README](https://github.com/danghai/Kernel/blob/master/character_device_driver/example3/README.md)|

### 1. Major and Minor Numbers

Char devices are accessed through names in the filesystem. Those names are called special files or device files or simply nodes of the filesystem tree; they are conventionally located in the `/dev` directory. Special files for char drivers are identified by a `"c"` in the first column of the ouput of command `ls -l`

```
crw-------  1 root root     10, 175 Sep  5 22:59 agpgart
crw-------  1 root root     10, 235 Sep  5 22:59 autofs
drwxr-xr-x  2 root root         620 Sep  5 22:59 block
drwxr-xr-x  2 root root          80 Sep  5 22:59 bsg
crw-------  1 root root     10, 234 Sep  5 22:59 btrfs-control
drwxr-xr-x  3 root root          60 Sep  5 22:59 bus
lrwxrwxrwx  1 root root           3 Sep  5 22:59 cdrom -> sr0
lrwxrwxrwx  1 root root           3 Sep  5 22:59 cdrw -> sr0
drwxr-xr-x  2 root root        3660 Sep  7 08:18 char
```

There are two numbers (separated by a comma). These numbers are the major and minor device number for particular device. Ex: Major number = 10, Minor number =  175, 235, ...

* `Major number: ` identifies the driver associated with the device. Modern Linux kernels allow multiple drivers to share major numbers, but most devices that you will see are still organized on the one-major-one-driver principle. To obtain the major use: `MAJOR (dev)`

* `Minor number: ` is used by the kernel to determine exactly which device is being referred to. Depending on how your driver is written, you can either get a direct pointer to your device from the kernel, or you can use the minor number yourself as an index into a local array of devices. To obtain the minor use: `MINOR (dev)`

Within the kernel, the `dev_t` type is used to hold device numbers( Major and Minor parts).If, instead, you have the major and minor numbers and need to turn them into a dev use:
`MKDEV (int major, int minor)`

### 2. The file_operations Structure

The file_operations structure is defined in `linux/fs.h` and holds pointers to functions defined by the driver that perform various operations on the device. For example, every character driver needs to define a function that reads from the device. The file_operations structure holds the address of the module's function that performs that operation. :

```c
struct file_operations {
   struct module *owner;
   loff_t (*llseek) (struct file *, loff_t, int);
   ssize_t (*read) (struct file *, char *, size_t, loff_t *);
   ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
   int (*readdir) (struct file *, void *, filldir_t);
   unsigned int (*poll) (struct file *, struct poll_table_struct *);
   int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
   int (*mmap) (struct file *, struct vm_area_struct *);
   int (*open) (struct inode *, struct file *);
   int (*flush) (struct file *);
   int (*release) (struct inode *, struct file *);
   int (*fsync) (struct file *, struct dentry *, int datasync);
   int (*fasync) (int, struct file *, int);
   int (*lock) (struct file *, int, struct file_lock *);
   ssize_t (*readv) (struct file *, const struct iovec *, unsigned long,loff_t *);
   ssize_t (*writev) (struct file *, const struct iovec *, unsigned long,loff_t *);
};
```

An instance of struct file_operations containing pointers to functions that are used to implement read, write, open, ... syscalls is commonly named `fops`

```c
static struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};
```

### 3. Registering A Device

One of the first things your driver will need to do when setting up a char device is to obtain one or more device numbers to work with. The necessary function for this task is `register_chrdev_region` 

```c
	int register_chrdev_region(dev_t first, unsigned int count, char *name);
```

`register_chrdev_region` works well if you know ahead of time exactly which device numbers you want. Often, however, you will not know which major numbers your device will use. The Kernel can allocate a mjor number by using a different function: 

```c
	int alloc_chrdev_region(dev_t *dev, unsigned int firstminor,unsigned int count, char *name);
```

Regardless of how you allocate your device numbers, you should free them when they are no longer in use. Device numbers are freed with: 

```c
	void unregister_chrdev_region(dev_t first, unsigned int count); 
```

The usual place to call register would be in your init module's, and the usual place to call unregister would be in your module's cleanup function.

### 4. Char Device Registration

As we mentioned, the kernel uses structures of type struct `cdev` to represent char devices internally. Before the kernel invokes your device's operations, you must allocate and register one or more of these structure. You want to embed the `cdev` structure within a device-specific structure of your own. You should initialize the structure that you have already allocated with:

```c
	void cdev_init(struct cdev *cdev, struct file_operation *fops);
```

Once the `cdev` structure is set up, the final step is to tell the kernel about it with a call to:

```c
	int cdev_add(struct cdev *dev, dev_t num, unsigned int count);
```

To remove a char device from the system call:

```c
	void cdev_del(struct cdev *dev);
```

Normally, when you do not want to allow something, you return an error code from the function which is supposed to do it. In the
 __exit module, It is impossible because it's a void function. However, there's a counter which keeps track of how many processes are using your module.
You can see what it's value is by looking at the 3rd field of `/proc/modules`. You should not use this counter directly,
but there are functions defined in `linux/module.h` which let you increase, decrease and display this counter. It is 
important to keep the counter accurate; if you ever do lose track of the correct usage count, you will never be able to
unload the module. 

```c
	try_module_get (THIS_MODULE)  	// Increment the use count
	module_put (THIS_MODULE)	// Decrement the use count.
``` 

### 5. Read and Write method

The `read` and `write` methods both perform a similar task, that is, copying data from and to application code.

```c
	ssize_t read (struct file *filp, char __user *buff, size_t count, loff_t *offp);
	ssize_t write(struct file *filp, const char__user *buff, size_t count, loff_t *offp);
```

For both methods, `filp` is the file pointer and `count` is the size of the requested data transfer. 
The `buff` argument points to the user buffer holding the data to be written or the empty buffer where the newly read
data should be placed. Finally, `offp` is a pointer to a long offset type object that indicates the file position the user
is accessing.

Obviously, your driver must be able to access the user-space buffer in order to get its job done. This access must always
be performed by special, kernel-supplied functions, however, in order to be safe. The code for `read` and `write` need to copy a whole segment of
data to or from the user address space. This capability is offered by the following kernel functions: 

```c
	unsigned long copy_to_user (void __user *to, const void *from, unsigned long count);
	unsigned long copy_from_user(void *to, const void __user *from, unsigned long count);
```

![The arguments to read](http://www.makelinux.net/ldd3/images/0596005903/figs/ldr3_0302.gif)

Both the read and write methods return a negative value if an error occurs. A return value greater than or 
equal to 0, instead, tells the calling program how many bytes have been successfully transferred. 

### 6. Example Character Device Driver
#### 1. [Example 1](https://github.com/danghai/Kernel/tree/master/character_device_driver/example1) 

The example code creates a char driver. You can `cat` its device file ( or open the file with a program) and the driver will put the number of times the device file has beeen read from into the file. We simply read in the data and print a message acknowledging that we received it. 

* [chardev.c](https://github.com/danghai/Kernel/blob/master/character_device_driver/example1/chardev.c)

* [README](https://github.com/danghai/Kernel/blob/master/character_device_driver/example1/README.md)


#### 2. [Example 2](https://github.com/danghai/Kernel/tree/master/character_device_driver/example2)

The code sample creates a char driver and instruction how to run the code. This code open charater device driver and read information about current value of data in `Kernel Space`. In addition, in `User Space`, it can pass argv[1] as a parameter to modify data in Kernel. Specifically, after reading, it will write the updated value data back to the `Kernel Space`, it can read and write data between `Kernel Space` and `User Space`

* [char.c](https://github.com/danghai/Kernel/blob/master/character_device_driver/example2/char.c)

* [user.c](https://github.com/danghai/Kernel/blob/master/character_device_driver/example2/user.c) 

* [README](https://github.com/danghai/Kernel/blob/master/character_device_driver/example2/README.md)

### Reference: 

[Video simple a character device](https://www.youtube.com/watch?v=E_xrzGlHbac)

[ A Character Device](http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/)

[The Linux Kernel Module Programming Guide](http://www.tldp.org/LDP/lkmpg/2.4/html/x579.html)



