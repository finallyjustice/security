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

//static unsigned int syscall_table_size;
static unsigned long *syscall_table_addr;
//static unsigned long *syscall_table_shadow;

static unsigned long *get_syscall_table(void)
{
	unsigned long *start;
	printk(KERN_ALERT "CLOSE: %d\n", __NR_close);
	printk(KERN_ALERT "sys_close: %016lx\n", (unsigned long)sys_close);
		/*if(start[__NR_close] == (unsigned long)sys_close)
		{
			return start;
		}*/
	return NULL;
}

static void check_syscall_table_handler(struct work_struct *w)
{
	printk(KERN_ALERT "Hello World\n");		
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

	syscall_table_addr = get_syscall_table();

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
	//kfree(dump_syscall_table);
	del_timer(&timer_s);
	printk(KERN_ALERT "exit the module\n");
}

MODULE_LICENSE("GPL");
module_init(hook_detect_init);
module_exit(hook_detect_exit);
