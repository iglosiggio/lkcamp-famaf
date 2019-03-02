/*
 * Copyright (C) 2013 Ezequiel Garcia.
 *
 * GPL... but who cares, it's just a Hello World!
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

static irqreturn_t int_handler(int irq, void *dev_id)
{
	pr_debug("Received an interrupt as pid=%d\n", current->pid);
	return 0;
}

static int hello_irq_init(void)
{
	int ret;

	ret = request_irq(8, int_handler, IRQF_SHARED, "foo", NULL);
	if (ret < 0)
		return -EINVAL;
	ret = request_irq(1, int_handler, IRQF_SHARED, "bar", NULL);
	if (ret < 0)
		return -EINVAL;
	return 0;
}

static void hello_irq_exit(void)
{
}

module_init(hello_irq_init);
module_exit(hello_irq_exit);

MODULE_AUTHOR("Ezequiel Garcia");
MODULE_DESCRIPTION("Typical Hello World! first interrupt example");
MODULE_LICENSE("GPL");
