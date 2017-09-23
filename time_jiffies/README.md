# Time Jiffies

### 1. Linux time

In the Linux kernel, time is measured by a global variable named `jiffies`, which identifies the number
of ticks that have occurred since the system was booted. The manner in which ticks are counted depends, as its
lowest level, on the particular hardware platform on which you are running; However, it is typically incremented
through an interrupt. The tick rate (jiffies's least significant bit) is configurable, but in a recent 2.6
kernel for x86, a tick euqals 4ms (250Hz).

Whenever your code needs to remember the current value of jiffies, it can simply access the unsigned long variable. You need to
read the current counter whenever your code needs to calculate a future time stamp, as shown in the following example:

```c
	j = jiffies;                      /* read the current value */
	stamp_1    = j + HZ;              /* 1 second in the future */ s
	tamp_half = j + HZ/2;             /* half a second */
	stamp_n    = j + n * HZ / 1000;   /* n milliseconds */
```

### 2. The timer API

Linux provides a simple API for the construction and management of timers. It consists of functions ( and helper functions)
for timer creation, cancellation, and management.

Timers are defined by the `timer_list` structure, which includes all of the data necessary to implement a timer
(including list pointers and optional timer statistics that are configured at compile time), and a user-provided
context. The user must then initialize the timer, which they can do in a few ways. The simplest method
is a call to `setup_timer`, which initializes the timer and sets the user provided callback function and context.
Otherwise, the user can set these value (function and data) in the timer and simply call `init_timer`. Note that
`init_timer` is called internally by `setup_timer`

```c
	void init_timer(struct timer_list *timer);
	void setup_timer (struct timer_list *timer, void(*function)(uunsigned long), unsigned long data);
```

With an initialized timer, the user now needs to set the expiration time, which is done through a call to `mod_timer`.
As users commonly provide an expiration in the futuure, they typicallly add `jiffies` here to offset from the current time.
Users can also delete a timer (if it has not expired) through a call to `del_timer`:

```c
	int mod_timer (struct timer_list *timer, unsigned)
	void del_timer (struct timer_list *timer);
```

### 3. Timer example

Let's look at some of these API functions in practice. Listing examples provide a simple kernel module that demonstrates the core aspects of the
simple timer API. Within `init_module`, you initialize a timer with `setup_timer` and then kick it off with a call to `mod_timer`.
When the timer expires, the callback function `timer_response` is invoked. Finally, the timer deletion (via del_timer) occurs when you remove the module.

| # | Title | Desciption |
| --- | --- | --- |
| 1 | [Example1](https://github.com/danghai/Kernel/blob/master/time_jiffies/time_example1.c) | Simple timer API, getting response timer from kernel depending on parameter|
| 2 | [Example2](https://github.com/danghai/Kernel/blob/master/time_jiffies/time_example2.c) | Simple timer API, getting response timer from kernel depending on factor|
| 3 | [Example3](https://github.com/danghai/Kernel/blob/master/time_jiffies/time_example3.c) | Blink LED Caps Locks with blink rate|

#### How to run:

### How to run

You'll need root on a Linux machine

```
# Build chardev.ko, the kernel module
$ make

# Load the kernel module that example you want
$ sudo insmod example2.ko

# See the log in kernel ,or you can open another terminal to watch log continuously
$dmesg
$dmesg -wH
```
