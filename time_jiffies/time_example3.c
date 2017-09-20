/*
*		time_example3.c: provides a simple kernel module that demonstrates the core aspects of the
* 	simple timer API. It takes parameter to set the blink_rate. The LED on Caps Lock will
*   blink with the blink_rate.
*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/time.h>
#include <linux/moduleparam.h>
#define CAPS_LED 4

MODULE_LICENSE("GPL/BSD");
MODULE_AUTHOR("Hai Dang Hoang");
MODULE_VERSION("0.1");

struct timer_list my_timer;

/*
*  Struct: factor for call mod_timer, Jiff saves current LED status
*/
static struct my_struct {
	unsigned char status_led ;
} my_str;

static int blink_rate = 2; // Set the default blink_rate
module_param(blink_rate,int,S_IRUSR | S_IWUSR);

static void timer_response(unsigned long data)
{

	struct my_struct *val = (struct my_struct *)data;
  if(blink_rate <=0)
  {
    printk(KERN_ERR "Blink_rate is negative ! Error");
    return;
  }

  /* Turn ON/OFF LED */
  /* Read keyboard status*/
  while((inb(0x64)&2)!=0){}
 /* Send command 0xED to port 0x60 to set LED*/
  outb(0xED,0x60);
  /* Read keyboard status again*/
  while((inb(0x64)&2)!=0){}
  /* Send value to LED on keyboard*/
  outb(val->status_led,0x60);

	/* now update with the data: toogle LED */
	val->status_led = val->status_led  ^ CAPS_LED ;

	/* and restart the timer: 1s blinking */
	mod_timer(&my_timer, ((jiffies/blink_rate) + HZ));
}

static int __init time_example2_init(void)
{
	printk(KERN_INFO "%s started, HZ=%d\n", __func__, HZ);

	my_str.status_led = 0;

	setup_timer(&my_timer, timer_response, (unsigned long)&my_str);

	mod_timer(&my_timer, ((jiffies/blink_rate) + (1 * HZ)));

	return 0;
}

static void __exit time_example2_exit(void)
{
	del_timer_sync(&my_timer);
	printk(KERN_INFO "unloaded\n");
}



module_init(time_example2_init);
module_exit(time_example2_exit);
