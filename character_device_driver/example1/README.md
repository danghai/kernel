# Example Character Device Driver

The example code creates a char driver. You can `cat` its device file ( or open the file with a program) and the driver will put the number of times the device file has beeen read from into the file. We simply read in the data and print a message acknowledging that we received it. 

### How to run

You'll need root on a Linux machine

```
# Build chardev.ko, the kernel module
$ make

# Load the kernel module 
$ sudo insmod chardev.ko

# Create the character device with Major: 246 and Minor: 0
$ sudo mknod /dev/chardev c 246 0

# Open the character by cat /dev/chardev and see the result
$ cat /dev/chardev
I already told you 0 times /dev/chardev file! 
$ cat /dev/chardev
I already told you 1 times /dev/chardev file! 
$ cat /dev/chardev
I already told you 2 times /dev/chardev file! 
$ cat /dev/chardev
I already told you 3 times /dev/chardev file! 
$ cat /dev/chardev
I already told you 4 times /dev/chardev file! 

$dmesg 

[186308.054216] Module loading and initial setup character driver .... 
[186308.054222] I was assigned major number 246. To talk to
[186308.054223] the driver, create a dev file with 
[186308.054224] 'mknod /dev/chardev c 246 0'.
[186308.054225] Try various minor numbers. Try to cat and echo to 
[186308.054226] the device file.
[186308.054226] Remove the device file and module when done .

```
