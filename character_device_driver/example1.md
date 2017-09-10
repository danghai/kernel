# Example Character Device Driver

The code sample creates a char driver and instruction how to run the code. This code open charater device driver and read information about current value of data in `Kernel Space`. In addition, in `User Space`, it can pass argv[1] as a parameter to modify data in Kernel. Specifically, after reading, it will write the updated value data back to the `Kernel Space`. It can read and write data between `Kernel Space` and `User Space`

### How to run

You'll need root on a Linux machine

```
# Build char.ko, the kernel module
# Build user.c, the user space
$ make

# Load the kernel module and Initial data = 7
$ sudo insmod char.ko data=7

# Check for the device number from the module's output
$ dmesg | tail -5

# Create the character device with Major: 246 and Minor: 0
$ sudo mknod /dev/chardev c 246 0

```

#### 1. You can test the function `Open`: 

```
$ cat /dev/chardev
$ dmesg | tail -5

```

The log file:

```
[128123.610902] Module loading and initial setup character driver .... 
[128123.610905] Allocated 7 devices at Major: 246 
[128500.328461] Successfully opened! 
[128500.328464] Currently, the value of data: 7
```

#### 2. You can test `read` and `write` function between user space and kernel space

```
$ sudo ./user 168
returned 7 from the system call, num bytes read: 4
successfully wrote 168 (8 bytes) to the kernel
$ dmesg | tail -1
[128672.755465] Good! Userspace wrote back the new data: 168 to kernel 
```
