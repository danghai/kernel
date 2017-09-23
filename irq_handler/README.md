# Interrupt Handler

### 1. Interrupt

* Under Linux, hardware interrupts are call IRQ. There are two types of IRQ's short and long.
A short IRQ is one which is expected to take a very short period of time, during which the rest
of the machine will be blocked and no other interrupts will be handled. A long IRQ is one which
can take longer, and during which other interrupts may occur (but not interrupt from the same device)
. If at all possible, it's better to declare an interrupt handler to be long.

* Each IRQ line is assigned a numeric value. For example, on the classic PC, IRQ `0` is the
timer interrupt and IRQ `1` is the keyboard interrupt.

* Some interrupts are dynamically assigned, such as interrups associated with devices on the
PCI bus.Other non-PC architectures have similar dynamic assignments for interrupt values.

* The kernel knows that a specific interrupt is associated with a specific device. The hardware
then issues interrupts to get the kernel's attention

### 2. What CPU do?

The following figure is a diagram of the path an interrupt takes through hardware and the kernel:

![diagram](https://notes.shichao.io/lkd/figure_7.1.png)

* When the CPU receives an interrupt, it stops whatever it's doing (unless it's processing a
more important interrupt, in which case it will deal with this one only when the more
important one is done), saves certain parameters on the stack and calls the interrupt handler

* For each interrupt line, the processor jumps to a unique location in memory
and executes the code located there. In this manner, the kernel knows the IRQ number
(the interrupt value) of the incoming interrupt. Then the kernel call `do_IRQ()`

* The interrupt handler usually read something from the hardware or send something to
the hardware, and then schedule the handling of the new information at a later time
(this is called the "bottom half") and return. The kernel is then guaranteed to
call the bottom half as soon as possible:

### 3. Interrupt Handlers

An `interrupt handler` or `interrupt service routine` (ISR) is the function that the kernel runs in response to a specific interrupt
Because an interrupt can occur at any time, an interrupt handler can be executed at any time. What differentiates interrupt
handlers from other kernel functions is that the kernel invokes them in response to interrupts and that they run in a special
context called `interrupt context`. This special context is occasionally called `atomic context` because code executing in this context is unable to block

The procession of interrupts is split into two parts, or `halves`:

![image Top and Bottom half](https://www.ibm.com/developerworks/linux/library/l-tasklets/figure1.gif)

* Top half: The interrupt handler is the top half. The top half is run immediately upon receipt of the interrupt and
performs only the work that is time-critical, such as acknowledging receipt of the interrupt or resetting the hardware

* Bottom half: Work that can be performed later is deferred until the bottom half. The bottom half runs in the future, at a more convenient time, with all interrupts enabled.

Registering an Interrupt Handler: Each device has one associated driver. If that device uses interrupts, that driver must register one interrupt handler
Drivers can register an interrupt handler and enable to given interrupt line with the function: `request_irq`
, which is declared in `<linux/interrupt.h`:

```c
	int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char *name, void *dev)
```

* irq: specifies the interrupt number to allocate, this value is typically hard-coded

* handler: is a function pointer to the actual interrupt handler that services this interrupt. This
function is invoked whenever the operating system receives the interrupt. Typedef:

```c
	irqreturn_t (*irq_handler_t)(int, void *);
```
Note the specific prototype of the handler function: It takes two parameters and has a return value of `irqreturn_t`.

* The third parameter, `flags`, can be either zero or a bit mask of one or more of the flags. You can see the more description in
[here](https://www.fsl.cs.sunysb.edu/kernel-api/re667.html)

### 4. Freeing an Interrupt Handler

When your driver unloads, you need to unregister your interrupt handler and disable the interrupt line by calling:

```c
	void free_irq(unsigned int irq, void *dev)
```
* If the specified interrupt line is not shared, this function removes the handler and siables the line.

* If the interrupt line is shared, the handler identified via `dev` is remove, but the interrupt line is disabled
only when the last handler is removed. With shared interrupt lines, a unique cookie is required to differentiate
between the multiple handlers that can exist on a single line and enable `free_irq()` to remove only the correct handler.

In either case (shared or unshared), if `dev` is non- `NULL`, it must match the desired handler. A call to `free_irq()` must be made from process context.

### !!! Example:

| # | Title | Desciption |
| --- | --- | --- |
| 1 | [irq_ex1](https://github.com/danghai/Kernel/blob/master/irq_handler/irq_ex1.c) | Simple Keyboards interrupt|
| 2 | [irq_ex2](https://github.com/danghai/Kernel/blob/master/irq_handler/irq_ex2.c) | Keyboards interrupt to turn ON/OFF Caps Lock LED|


#### 1. [irq_ex1.c](https://github.com/danghai/Kernel/blob/master/irq_handler/irq_ex1.c) : Keyboards interrupt

This code binds itself to `IRQ` 1, which is the IRQ of the keyboard controlled under Intel architectures. Then,
when it receives a keyboard interrupt, it reads the keyboard's status and the scan code, which is the value return
by the keyboard. And then, it puts information about Key that pressed. This example only has 3 key: ESC, F1 and F2
Scancode keyboard you can find this link: [PS/2 Keyboard](http://wiki.osdev.org/PS/2_Keyboard)

Note: The code is completely Intel specific. If you are not running on an Intel platform, it will not work. The code
has a bug on `rmmod`. There is no way to restore after `rmmod`. Before `insmod`, do on another terminal `sleep 150 ; reboot`.
I do not know how to fix it. If you can fix it in, please let me know ! 

#### 2. [irq_ex2.c](https://github.com/danghai/Kernel/blob/master/irq_handler/irq_ex2.c) : Keyboards interrupt to turn ON/OFF Caps Lock LED

irq_ex2.c: An interrupt handler example. This code binds itself to `IRQ` 1, which
is the IRQ of the keyboard controlled under Intel architectures. Then, when it
receives a keyboard interrupt, it reads the information about status led and keycode
into the work queue. Pressing:

       * ESC   ---> Caps Lock Led OFF

       * F1,F2 ---> Caps Lock Led ON




### Reference:

[Interrupts and Interrupt Handlers](https://notes.shichao.io/lkd/ch7/)

[PS/2 Keyboard](http://wiki.osdev.org/PS/2_Keyboard)

[Interrupt Handler](http://www.tldp.org/LDP/lkmpg/2.4/html/x1210.html)
