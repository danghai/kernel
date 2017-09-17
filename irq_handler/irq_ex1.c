/*
* An interrupt handler
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#define MY_WORK_QUEUE_NAME "WQsched.c"

static struct workqueue_struct *my_workqueue;
/*
*  This will get called by the kernel as soon as it's safe
* to do everything normally allowed by kernel modules
*/

static void got_char (void *scancode)
{
    printk(KERN_INFO "Scan Code %x %s. \n",(int) *((char *)scancode)
          & 0x7F, *((char *)scancode) & 0x80 ? "Released" : "Pressed");
}

/*
*   This function services keyboard interrupts. It reads the relevant
*   information from the keyboard and then puts the non time critical
*   part into the work queue. This will be run when the kernel considers it safe
*/

irqreturn_t irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
/*
* This variables are static because they need to be
* accessible (through pointers) to the bottom half routine.
*/
static int initialised = 0;
static unsigned char scancode;
static struct work_struct task;
unsigned char status;

/*
* Read keyboard status
*/
status = inb(0x64);
scancode = inb(0x60);

if (initialised == 0)
{
    INIT_WORK(&task, got_char);
    initialised = 1;
}
  queue_work(my_workqueue, &task);
  return IRQ_HANDLED;
}

/*
* Initialize the module - register the IRQ handler
*/
int init_module()
{
    my_workqueue = create_workqueue(MY_WORK_QUEUE_NAME);
    free_irq(1,NULL);
    return request_irq (1, irq_handler,IRQF_SHARED, "test_keyboard_irq_handler",(void *)(irq_handler));
}

void cleanup_module()
{
    free_irq(1,NULL);
}

MODULE_LICENSE("GPL");
