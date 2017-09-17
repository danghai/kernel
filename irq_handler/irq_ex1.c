/*
* An interrupt handler
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hai Dang Hoang");

/*
*   This function services keyboard interrupts. It reads the relevant
*   information from the keyboard and then puts information about Key that pressed
*   This example only has 3 key: ESC, F1 and F2
*/

irqreturn_t irq_handler(int irq, void *dev_id, struct pt_regs *regs)
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
              break;
  case 0x3B:  printk (KERN_INFO "! You pressed F1 ...\n");
              break;
  case 0x3C:  printk (KERN_INFO "! You pressed F2 ...\n");
              break;
  default:
              break;
}

  return IRQ_HANDLED;
}

/*
* Initialize the module - register the IRQ handler
*/
static int __init irq_ex_init(void)
{
    /* Free interrupt*/
    free_irq(1,NULL);
    return request_irq (1, (irq_handler_t) irq_handler,IRQF_SHARED, "test_keyboard_irq_handler",(void *)(irq_handler));
}

static void __exit irq_ex_exit(void)
{
    free_irq(1,NULL);
}

module_init(irq_ex_init);
module_exit(irq_ex_exit);
