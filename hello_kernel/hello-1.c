/*
* Name: Hai Dang Hoang
* hello-1.c: - The simplest kernel module
*/

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
