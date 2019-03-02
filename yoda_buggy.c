/*
 * Yoda's char device driver
 *
 * Copyright (C) 2013 Ezequiel Garcia.
 *
 * GPL applies: you know what that means.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

static int major;

static ssize_t yoda_read_buggy(struct file *file, char __user *buf,
			       size_t count, loff_t *offset)
{
	const char *yoda_string = "Completely fixed, this string is.\n";

	pr_info("user wants to read %zd bytes at %lld\n", count, *offset);

	if (copy_to_user(buf, yoda_string, strlen(yoda_string)))
		return -EFAULT;
	return strlen(yoda_string);
}

static const struct file_operations fops = {
	.read = yoda_read_buggy,
};

static int yoda_init(void)
{
	major = register_chrdev(0, KBUILD_MODNAME, &fops);
	if (major < 0)
		return major;

	pr_info("device registered with major %d\n", major);
	return 0;
}

static void yoda_exit(void)
{
	return unregister_chrdev(major, KBUILD_MODNAME);
}

module_init(yoda_init);
module_exit(yoda_exit);

MODULE_AUTHOR("Ezequiel Garcia");
MODULE_DESCRIPTION("A simple read-only char device driver");
MODULE_LICENSE("GPL");
