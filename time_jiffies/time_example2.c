/*
*		time_example2.c: provides a simple kernel module that demonstrates the core aspects of the
* 	simple timer API. Within `init_module`, you initialize a timer with `setup_timer` and then kick it off with a call to `mod_timer`.
* 	When the timer expires depending on factor, the callback function `timer_response` is invoked. The timer response will be :
* 	1s --> 2s --> 3s --> 4s ....
*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/time.h>

MODULE_LICENSE("GPL/BSD");
MODULE_AUTHOR("Hai Dang Hoang");
MODULE_VERSION("0.1");
struct timer_list my_timer;

/*
*  Struct: factor for call mod_timer, Jiff saves current jiffies
*/
static struct my_struct {
	int factor;
	unsigned long jiff;
} my_str;

static void timer_response(unsigned long data)
{
	struct my_struct *val = (struct my_struct *)data;

	printk(KERN_INFO "factor=%u elapsed time = %lu\n",
		val->factor, (jiffies - val->jiff));

	/* now update with the data */
	val->factor++;
	val->jiff = jiffies;

	/* and restart the timer */
	mod_timer(&my_timer, (jiffies + (val->factor * HZ)));
}

static int __init time_example2_init(void)
{
	printk(KERN_INFO "%s started, HZ=%d\n", __func__, HZ);

	my_str.factor = 1;
	my_str.jiff = jiffies;

	setup_timer(&my_timer, timer_response, (unsigned long)&my_str);

	mod_timer(&my_timer, (jiffies + (1 * HZ)));

	return 0;
}

static void __exit time_example2_exit(void)
{
	del_timer_sync(&my_timer);
	printk(KERN_INFO "unloaded\n");
}



module_init(time_example2_init);
module_exit(time_example2_exit);
