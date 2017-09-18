
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hai Dang Hoang");

/*
*   Define const for LED on keyboard
*/
#define SCROLL_LED 1
#define NUM_LED 2
#define CAPS_LED 4

/* Prototype set led on keyboard */
void set_led(int, int, int);
struct timer_list my_timer;

/*
*   Function: set_led: Set status LED on keyboard
*/
void set_led(int scroll_led, int num_led, int caps_led)
{
  unsigned char status_led =0;
//  static unsigned char scancode;
//  unsigned char status;
  /* Take argument to set status_led */
  status_led = (scroll_led) ? (status_led | SCROLL_LED):(status_led & SCROLL_LED);
  status_led = (num_led) ? (status_led | NUM_LED):(status_led & NUM_LED);
  status_led = (caps_led) ? (status_led | CAPS_LED):(status_led & CAPS_LED);

  /* Read keyboard status*/
   while((inb(0x64)&2)!=0){}
  /* Send command 0xED to port 0x60 to set LED*/
  outb(0xED,0x60);
  /* Read keyboard status again*/
  while((inb(0x64)&2)!=0){}
  /* Send value to LED on keyboard*/
  outb(status_led,0x60);
  printk(KERN_INFO "Write status_led: %d \ns",status_led);
}


irqreturn_t irq_handler(int irq, void *dev_id)
{
  /*
* This variables are static because they need to be
* accessible (through pointers) to the bottom half routine.
*/

  static unsigned char scancode;
  unsigned char status;

/*
* Read keyboard status
*/
  status = inb(0x64);
  scancode = inb(0x60);

switch (scancode)
{
  case 0x01:  printk (KERN_INFO "! You pressed Esc ...\n");
              set_led(0,0,0);
              break;
  case 0x3B:  printk (KERN_INFO "! You pressed F1 ...\n");
              set_led(1,1,1);
              break;
  case 0x3C:  printk (KERN_INFO "! You pressed F2 ...\n");
              set_led(0,0,1);
              break;
  default:
              break;
}

  return IRQ_HANDLED;
}

static int __init irq_ex_init (void)
{
    free_irq(1,NULL);
    return request_irq(1,(irq_handler_t) irq_handler, IRQF_SHARED,"test_keyboard_irq_handler",(void *)(irq_handler));
}

static void __exit irq_ex_exit (void)
{
    printk(KERN_INFO "! Module is unload ...\n");
    free_irq(1,NULL);
}

module_init(irq_ex_init);
module_exit(irq_ex_exit);
