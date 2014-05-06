/** File:		main.c
 ** Author:		Dongli Zhang
 ** Contact:	dongli.zhang0129@gmail.com
 **
 ** Copyright (C) Dongli Zhang 2013
 **
 ** This program is free software;  you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 2 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY;  without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 ** the GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program;  if not, write to the Free Software 
 ** Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

#define TIME_SLEEP 3000 /* in msec */

static struct timer_list timer_s;
static struct workqueue_struct *wq;

static unsigned int syscall_table_size;
static unsigned long *syscall_table_addr;
static unsigned long *syscall_table_copy;

static unsigned int get_syscall_table_size(void)
{
	unsigned int size = 0;

	while(syscall_table_addr[size])
		size++;
	return size*sizeof(unsigned long *);
}

static void check_syscall_table_handler(struct work_struct *w)
{
	//printk(KERN_ALERT "Hello World\n");	
	unsigned int sys_num = 0;

	while(syscall_table_addr[sys_num])
	{
		if(syscall_table_addr[sys_num] != syscall_table_copy[sys_num])
		{
			printk(KERN_ALERT "ALERT: syscall hook detected on %u\n", sys_num);
		}
		sys_num++;
	}
}
static DECLARE_DELAYED_WORK(check_syscall_table, check_syscall_table_handler);

static void my_timer_handler(unsigned long data)
{
	unsigned long onesec;

	onesec = msecs_to_jiffies(1000);
	queue_delayed_work(wq, &check_syscall_table, onesec);

	if(mod_timer(&timer_s, jiffies+msecs_to_jiffies(TIME_SLEEP)))
		printk(KERN_ALERT "failed to continue to set timer again\n");
}

static int __init hook_detect_init(void)
{
	printk(KERN_ALERT "init the module\n");

	syscall_table_addr = (unsigned long *)0xffffffff81a001a0;
	syscall_table_size = get_syscall_table_size();
	syscall_table_copy = kmalloc(syscall_table_size, GFP_KERNEL);
	if(!syscall_table_copy)
	{
		printk(KERN_ALERT "Failed to allocate memory for syscall table\n");
		return -ENOMEM;
	}
	memcpy(syscall_table_copy, syscall_table_addr, syscall_table_size);

	wq = create_singlethread_workqueue("syscall_hook_detect");

	setup_timer(&timer_s, my_timer_handler, 0);
	if(mod_timer(&timer_s, jiffies+msecs_to_jiffies(TIME_SLEEP)))
	{
		printk(KERN_ALERT "failed to setup my timer\n");
		return -ECANCELED;
	}

	return 0;
}

static void __exit hook_detect_exit(void)
{
	if(wq)
		destroy_workqueue(wq);
	del_timer(&timer_s);
	kfree(syscall_table_copy);
	
	printk(KERN_ALERT "exit the module\n");
}

MODULE_LICENSE("GPL");
module_init(hook_detect_init);
module_exit(hook_detect_exit);
