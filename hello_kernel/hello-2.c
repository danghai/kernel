/*
*  Hello-2.c - Demonstrates command line argument passing to a module
*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL/BSD");
MODULE_AUTHOR("Hai Dang Hoang");

static int my_int = 1;
static char *my_string = "haidang";
static int my_int_array[2] = {0,1};
static int ptr = 0;

/*
 Declare the module param name:
 module_param(name,type,perm)
 module_param_array(name, type, num, perm)
 1) The first param is the parameters Name
 2) THe second param is the it's data types
 3) The third argument is the permissions bits
 For exposing parameters in sysfs (if non-zero) at a later stage
 4) the fourth (module_param_array): is a pointer to the variable that
 will store the number of elements of the array initialized by the user at module loading time
*/

module_param(my_int,int, S_IRUSR | S_IWUSR);
module_param(my_string, charp, S_IRUSR | S_IWUSR);
module_param_array(my_int_array,int,&ptr,  S_IRUSR | S_IWUSR);

static int __init hello_init(void)
{
    int i;
    printk( KERN_INFO "Hello Kernel ! \n");
    printk (KERN_INFO "My int is : %d \n", my_int);
    printk (KERN_INFO "My string is: %s \n", my_string);
    for ( i =0 ; i < 2;i++)
        printk(KERN_INFO "my_int_array[%d] = %d \n",i,my_int_array[i]);

    return 0;
}

static void __exit hello_exit(void)
{
    printk (KERN_INFO "Goodbye, Kernel \n");
}

module_init (hello_init);
module_exit (hello_exit);
