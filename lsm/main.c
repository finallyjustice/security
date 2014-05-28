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
#include <linux/security.h>
#include <linux/file.h>
#include <linux/fs.h>

int lsm_file_permission(struct file *file, int mask)
{
	const char *name = file->f_path.dentry->d_name.name;
	if(!strcmp(name, "test.txt"))
	{
		file->f_flags |= O_RDONLY;
		printk(KERN_ALERT "Dongli: LSM Error\n");
	}
	return 0;
}

static struct security_operations lsm_security_ops = {
	.name = "test_lsm",
	.file_permission = lsm_file_permission,
};

static int __init test_lsm_init(void)
{
	printk(KERN_ALERT "init the module\n");

	if(register_security(&lsm_security_ops))
		printk(KERN_ALERT "Dongli: lsm registration failed\n");

	return 0;
}

static void __exit test_lsm_exit(void)
{
	printk(KERN_ALERT "exit the module\n");

	//if(unregister_security(&lsm_security_ops))
	//{
	//	printk(KERN_ALERT "unregister error\n");
	//}
}

MODULE_LICENSE("GPL");
module_init(test_lsm_init);
module_exit(test_lsm_exit);
