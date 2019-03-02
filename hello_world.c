/*
 * Copyright (C) 2013 Ezequiel Garcia.
 *
 * GPL... but who cares, it's just a Hello World!
 */

/* Nice standarization trick. Must be *before* headers to take effect. */
#if 1
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#endif

#include <linux/kernel.h>
#include <linux/module.h>

static int hello_init(void)
{
	pr_info("Hello world! This module is now loaded\n");

	/* Success! */
	return 0;
}

static void hello_exit(void)
{
	pr_info("Good-bye world! Unloading ...\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Ezequiel Garcia");
MODULE_DESCRIPTION("Typical Hello World! first example");

/*
 * Uncomment the code below to prevent a kernel taint upon module loading.
 * Needless to say, a kernel taint is something bad and only developers with
 * crappy jobs at closed source companies get them :-)
 */
#if 0
MODULE_LICENSE("GPL");
#endif
