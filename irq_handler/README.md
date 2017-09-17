# Interrupt Handler

### 1.Interrupt

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

![diagram](https://www.google.com/url?sa=i&rct=j&q=&esrc=s&source=imgres&cd=&cad=rja&uact=8&ved=0ahUKEwjzktvD7KvWAhXIPiYKHepqBcEQjRwIBw&url=https%3A%2F%2Fnotes.shichao.io%2Flkd%2Fch7%2F&psig=AFQjCNEMfFbri77l00UVw_JrJmhq6Z8I7w&ust=1505724908321754)

When the CPU receives an interrupt, it stops whatever it's doing (unless it's processing a
more important interrupt, in which case it will deal with this one only when the more 
important one is done), saves certain parameters on the stack and calls the interrupt handler

For each interrupt line, the processor jumps to a unique location in memory 
and executes the code located there. In this manner, the kernel knows the IRQ number
(the interrupt value) of the incoming interrupt. Then the kernel call `do_IRQ()`

The interrupt handler usually read something from the hardware or send something to
the hardware, and then schedule the handling of the new information at a later time
(this is called the "bottom half") and return. The kernel is then guaranteed to
call the bottom half as soon as possible
