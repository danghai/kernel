/*
*   time_example1.c: It is a simple kernel module that demonstrates
*   the core aspects of the simple timer API
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#define SUCCESS 0

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hai Dang Hoang");

static struct timer_list my_timer;

void my_timer_response (unsigned long data)
{
  printk(KERN_INFO "My timer response (%ld) . \n",jiffies);
}

static int __init time_example1_init(void)
{
  int ret;
  printk(KERN_INFO "Timer module installing ...\n");
  setup_timer (&my_timer, my_timer_response, 0);

  printk(KERN_INFO "Starting timer to fire in 200ms (%ld) \n",jiffies);
  ret = mod_timer(&my_timer,jiffies + 3*HZ);
  if (ret)
    printk(KERN_INFO "Error in mod_timer\n");
  return SUCCESS;
}

static void __exit time_example1_exit(void)
{
  int ret;
  ret = del_timer( &my_timer );
  if (ret)
    printk("The timer is still in use...\n");

  printk("Timer module uninstalling ...\n");
}

module_init(time_example1_init);
module_exit(time_example1_exit);
