#include <linux/module.h>
#include <linux/types.h>
#include <linux/time.h>

/* what's wrong with this? */
struct timer_list example_timer;

static struct my_local_struct {
	int foo;
	unsigned long jiff;
} my_str;

static void example_timer_cb(unsigned long data)
{
	struct my_local_struct *val = (struct my_local_struct *)data;

	printk(KERN_INFO "foo=%u elapsed time = %lu\n",
		val->foo, (jiffies - val->jiff));

	/* now mess with the data */
	val->foo++;
	val->jiff = jiffies;

	/* and restart the timer */
	mod_timer(&example_timer, (jiffies + (val->foo * HZ)));
}

static int __init time_example2_init(void)
{
	printk(KERN_INFO "%s started, HZ=%d\n", __func__, HZ);

	my_str.foo = 1;
	my_str.jiff = jiffies;

	setup_timer(&example_timer, example_timer_cb, (unsigned long)&my_str);

	mod_timer(&example_timer, (jiffies + (1 * HZ)));

	return 0;
}

static void __exit time_example2_exit(void)
{
	del_timer_sync(&example_timer);
	printk(KERN_INFO "unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hai Dang Hoang");
MODULE_VERSION("0.1");

module_init(time_example2_init);
module_exit(time_example2_exit);
