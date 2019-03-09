/*
 * A simple (and probably semantically broken if you get pedantic
 * on what a fifo really is) fifo-like char device just to show how
 * the linux API list works.
 *
 * Copyright (C) 2013 Ezequiel Garcia.
 *
 * Some bugs are intentional, some aren't. In particular there's
 * two *major* issues with this driver, but I don't want to ruin
 * the bug hunting.
 *
 * And a fun hint: How many times can you do this successfully?
 *
 *   $ dd if=/dev/zero of=fifo bs=10M count=1
 *
 * GPL applies: you know what that means.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

struct fifo_entry {
	struct list_head list;
	char *buffer;
	size_t size;
};

static struct list_head fifo_list;
static int major;

static ssize_t fifo_read(struct file *file, char __user *buf,
				size_t count, loff_t *offset)
{
	struct fifo_entry *entry;
	struct list_head *list;
	size_t to_read;
	loff_t entry_off = *offset;
	bool found = false;

	pr_info("user wants to read %zd bytes at %lld\n", count, *offset);

	/* Find the buffer slot where the requested offset is located */
	list_for_each(list, &fifo_list) {
		/*
		 * Some C pointer magic. This is dirty and since kernel
		 * developers are elegant folks, they've exposed a handsome
		 * way of doing this: search the headers!
		 */
		entry = (struct fifo_entry *)list;

		if (entry_off < entry->size) {
			found = true;
			break;
		}
		entry_off -= entry->size;
	}

	/*
	 * We control the offset, if it moved past our allocated memory then it
	 * has to be located right at the start of the 'next block'.
	 *
	 * If this is the case then we signal the calling task to retry the
	 * read operation and pray for a correct interpretation of the error.
	 */
	if (!found)
		return -EAGAIN;

	to_read = min(count, (size_t)(entry->size - entry_off));

	if (copy_to_user(buf, entry->buffer + entry_off, to_read))
		return -EFAULT;

	*offset += to_read;
	return to_read;
}

static ssize_t fifo_write(struct file *file, const char __user *buf,
			  size_t count, loff_t *offset)
{
	struct list_head *list;
	struct fifo_entry *entry;
	loff_t end_offset = count;
	loff_t block_offset = 0;
	int ret;

	pr_info("user wants to write %zd bytes at %lld\n", count, *offset);

	/*
	 * If we want to write to the pipe then our offset needs to be advanced
	 * to the end of our allocated blocks.
	 */
	list_for_each(list, &fifo_list) {
		entry = (struct fifo_entry *)list;
		end_offset += entry->size;
	}

	pr_info("going to effectively write them to %lld\n", end_offset - count);

	entry = kzalloc(sizeof(struct fifo_entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	/* Allocate a buffer to accommodate the user write request */
	entry->size = count;
	entry->buffer = kzalloc(count, GFP_KERNEL);
	if (!entry->buffer) {
		kzfree(entry);
		return -ENOMEM;
	}

	ret = simple_write_to_buffer(entry->buffer, entry->size,
				&block_offset, buf, count);
	if (ret < 0) {
		kzfree(entry->buffer);
		kzfree(entry);
		return ret;
	}

	/*
	 * If everything went right then we update our list and move our offset
	 */
	list_add_tail(&entry->list, &fifo_list);
	*offset = end_offset;
	return ret;
}

static const struct file_operations fops = {
	.read = fifo_read,
	.write = fifo_write,
};

static int fifo_init(void)
{
	INIT_LIST_HEAD(&fifo_list);

	major = register_chrdev(0, KBUILD_MODNAME, &fops);
	if (major < 0)
		return major;

	pr_info("device registered with major %d\n", major);
	return 0;
}

static void fifo_exit(void)
{
	struct fifo_entry *entry;
	struct list_head *list, tmp;
	list_for_each_safe(list, tmp, &fifo_list) {
		entry = (struct fifo_entry*)list;
		list_del(entry);

		/*
		 * What happens if someone tries to write 0 bytes?
		 * Can we always free our list?
		 */
		kzfree(entry->buffer);
		kzfree(entry);
	}
	return unregister_chrdev(major, KBUILD_MODNAME);

}

module_init(fifo_init);
module_exit(fifo_exit);

MODULE_AUTHOR("Ezequiel Garcia");
MODULE_DESCRIPTION("Another writable char device driver");
MODULE_LICENSE("GPL");
