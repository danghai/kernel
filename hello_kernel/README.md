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

Despite what you might think, `printk()` was not meant to communicate information to the user, even though we used it for exactly this purpose in hello-1. It happens to be a logging mechanism for the kernel, and is used to log information or give warnings. Therefore, each `printk()` statement comes with a priority, which is the <1> and `KERN_ALERT`. There are 8 priorities and the kernel has macros for them, so you do not have to use cryptic numbers, and you can view them (and their meanings) in `linux/kernel.h`. If you do not specify a priority level, the default priority, `DEFAULT_MESSAGE_LOGLEVEL`, will be used.

### 3. Compiling Kernel Modules

Kernel modules need to be compiled a bit differently from regular userspace apps. Former kernel versions required us to care much about these  setting, which are usually stored in [Makefiles](https://github.com/danghai/Kernel/blob/master/hello_kernel/Makefile). Although hierarchically organized, many redundant setting accumulated in sublevel Makefiles and made them large and rather difficult to maintain. Fortunately, there is a new way of doing these things, called kbuild, and the build process for external loadable modules is now fully integrated into the standard kernel build mechanism. To learn more on how to compile modules, see file [https://www.kernel.org/doc/Documentation/kbuild/modules.txt](https://www.kernel.org/doc/Documentation/kbuild/modules.txt). Additional details about `Makefile` for kernel modules are available in [linux/Documentation/kbuild/makefiles.txt](linux/Documentation/kbuild/makefiles.txt).

You can compile the module by issuing the command `make`. And then, you can insert your compiled module it into the kernel with `insmod` command. To see the log message, we can use the command `dmesg`. (Note: you must use root privileges to insmod module kernel). In order to remote module from kernel, you can use `rmmod` command. It should be

```
danghai@ubuntu:~/Kernel/hello_kernel$ make
make -C /lib/modules/4.4.0-45-generic/build M=/home/danghai/Kernel/hello_kernel modules
make[1]: Entering directory '/usr/src/linux-headers-4.4.0-45-generic'
  CC [M]  /home/danghai/Kernel/hello_kernel/hello-1.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /home/danghai/Kernel/hello_kernel/hello-1.mod.o
  LD [M]  /home/danghai/Kernel/hello_kernel/hello-1.ko
make[1]: Leaving directory '/usr/src/linux-headers-4.4.0-45-generic'

danghai@ubuntu:~/Kernel/hello_kernel$ sudo insmod hello-1.ko
danghai@ubuntu:~/Kernel/hello_kernel$ sudo rmmod hello-1.ko
```

5 tail lines in `dmesg`: 

```
[10708.132821] hello_1: module license 'GPL/BSD' taints kernel.
[10708.132823] Disabling lock debugging due to kernel taint
[10708.133280] hello_1: module verification failed: signature and/or required key missing - tainting kernel
[10708.139078] Hello kernel...
[10913.125658] Goodbye kernel!
```

### 4. The __init and __exit Macros

The module initialization function registers any facility offered by the module.
Initialization functions should be declared `static`, since they are not meant
to be visible outside the specific file. The __init token in the definition may look a little strange; it is a hint to the kernel that the given function is used only at initialization time. The module loader drops the initialization function after the module is loaded, makin gits memory available for other uses. The use of `module_init` is mandatory. This macro adds a special section to the module's object code starting where the module's initialization function is to be found. Without this definition, your initialization function is never called.

The cleanup function has no value to return, so it is declared void. The __exit modifier marks the code as being for module unload only (by causing the compiler to place it in a special ELF section). If your module is build directly into the kernel or if your kernel is configured to disallow the unloading of modules, functions marked __exit are simply discarded. If your module does not define a cleanup function, the kernel does not allow it to be unloaded.

### 5. [hello-2.c](https://github.com/danghai/kernel/blob/master/hello_kernel/hello-2.c) Passing Command Line Arguments to a Module

Modules can take command line arguments, but not with the `argc/argv`. To allow arguments to be passed to your module, declare the variables that will take the values of the command line arguments as global and then use the `module_param()` macro (defined in linux/moduleparam.h) to set the mechanism up. At runtime, insmod will fill the variables with any command line arguments that are given, (Ex: insmod hello-2.ko my_variable=7). The variable declarations and macros should be placed at the beginning of the module for clarity. 

The `module_param()` macro takes 3 arguments: the name of the variable, its type and permission for the corresponding file in sysfs. If you would like to use arrays of integers of string you need to use `module_param_array()` and `module_param_string()` (in module_param_array has 4 argument, the fourth argument is a pointer to the variable that will store the number of elements of the array initialized by the user at module loading time). Example run: 

```
danghai@ubuntu:~/Kernel/hello_kernel$ sudo insmod hello-2.ko my_int=7 my_string="Trammie_Le_Duong" my_int_array=-1
danghai@ubuntu:~/Kernel/hello_kernel$ dmesg
[48914.179089] Hello Kernel ! 
[48914.179092] My int is : 7 
[48914.179093] My string is: Trammie_Le_Duong 
[48914.179093] my_int_array[0] = -1 
[48914.179094] my_int_array[1] = 1 
```







