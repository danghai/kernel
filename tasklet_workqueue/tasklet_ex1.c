#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>

MODULE_LICENSE ("GPL");
MODULE_AUTHOR(" Hai Dang Hoang ");

char my_tasklet_data[] = "my_tasklet_function was called";

/* Bottom Half Function */
void my_tasklet_function (unsigned long data)
{
    printk("%s\n", (char *)data);
    return;
}

DECLARE_TASKLET(my_tasklet, my_tasklet_function,(unsigned long) &my_tasklet_data);

static int __init tasklet_ex1_init(void)
{
    /* Schedule the Bottom Half */
    printk (KERN_INFO "Module load ... \n");
    tasklet_schedule (&my_tasklet);
    return 0;
}

static void __exit tasklet_ex1_exit(void)
{
    /* Stop the tasklet before exit */
    tasklet_kill(&my_tasklet);
    printk (KERN_INFO "Module unload ... \n");
    return;
}

module_init(tasklet_ex1_init);
module_exit(tasklet_ex1_exit);
