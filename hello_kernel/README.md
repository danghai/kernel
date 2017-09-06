# Hello Kernel

### 1. [hello-1.c](https://github.com/danghai/kernel/blob/master/hello_kernel/hello-1.c)

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
MODULE_LICENSE("GPL/BSD");
int __init hello_init(void)
{
    printk(KERN_INFO "Hello kernel...\n");
    return 0;
}

void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye kernel!\n");
}

MODULE_AUTHOR("Hai Dang Hoang");
module_init(hello_init);
module_exit(hello_exit);
```

Kernel modules must have at least two functions: a "start" (initialization) function called `module_init` which is called when the module is insmod into the kernel, and an "end" function called `module_exit`, which is called just before it is rmmod.

Every kernel module needs to include `linux/module.h`. We need to include `linux/kernel.h` only for the macro expasion for the `printk()` log level.

### 2. Introducing printk()

Despite what you might think, `printk()` was not meant to communicate information to the user, even though we used it for exactly this purpose in hello-1. It happens to be a logging mechanism for the kernel, and is used to log information or give warnings. Therefore, each `printk()` statement comes with a priority, which is the <1> and `KERN_ALERT`. There are 8 priorities and the kernel has macros for them, so you do not have to use cryptic numbers, and you can view them (and their meanings) in `linux/kernel.h`. If you do not specify a priority level, the default priority, `DEFAULT_MESSAGE_LOGLEVEL`, will be used
