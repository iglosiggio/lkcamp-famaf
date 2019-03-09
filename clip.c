/*
 * Some simple clipboard-like char device driver
 * to show how write works.
 *
 * Copyright (C) 2013 Ezequiel Garcia.
 *
 * GPL applies: you know what that means.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>

struct clip {
	char *buffer;
	size_t count;
};

static int major;
static struct clip *clip;

static ssize_t clip_read(struct file *file, char __user *buf,
				size_t count, loff_t *offset)
{
	pr_info("user wants to read %ld bytes at %lld\n", count, *offset);

	return simple_read_from_buffer(buf, count, offset,
				       clip->buffer, clip->count);
}

static ssize_t clip_write(struct file *file, const char __user *buf,
			  size_t count, loff_t *offset)
{
	void* new_buffer;

	pr_info("user wants to write %ld bytes at %lld\n", count, *offset);

	/* Rellocate a buffer to accomodate the user write request */
	new_buffer = krealloc(clip->buffer, *offset + count, GFP_KERNEL);

	/*
	 * What's the appropiate way to react to this error?
	 * Should we free the allocated buffer?
	 * Should we zero the allocated buffer?
	 * Should we let the old and still allocated buffer intact?
	 */
	if (!new_buffer)
		return -ENOMEM;

	clip->buffer = new_buffer;
	clip->count = *offset + count;

	return simple_write_to_buffer(clip->buffer, clip->count, offset,
				      buf, count);
}

static const struct file_operations fops = {
	.read = clip_read,
	.write = clip_write,
};

static int clip_init(void)
{
	/*
	 * First of all, we need some clip-specific structure
	 * to hold the driver's state.
	 */
	clip = kzalloc(sizeof(struct clip), GFP_KERNEL);

	/*
	 * Hey! This is no user-space: kernel memory allocations can
	 * actually fail so you *need* to check this if you don't
	 * want to bring your system down.
	 */
	if (!clip)
		return -ENOMEM;

	major = register_chrdev(0, KBUILD_MODNAME, &fops);
	if (major < 0)
		return major;
	pr_info("device registered with major %d\n", major);

	return 0;
}

static void clip_exit(void)
{
	if (clip->buffer)
		kfree(clip->buffer);
	kfree(clip);

	/*
	 * Is it correct to free structure before the unregistering
	 * the char device? Maybe the word 'preemption' says anything to you...
	 */
	return unregister_chrdev(major, KBUILD_MODNAME);
}

module_init(clip_init);
module_exit(clip_exit);

MODULE_AUTHOR("Ezequiel Garcia");
MODULE_DESCRIPTION("Writable char device driver");
MODULE_LICENSE("GPL");
