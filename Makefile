# Objects must be listed here, using this 'obj-m' magic variable
obj-m += hello_world.o \
         hello_irq.o \
	 yoda_buggy.o \
	 yoda_works.o \
	 yoda_superb.o \
	 clip.o \
	 fifo.o

default:
	@echo "Hello dummy, which module do you want to build now?"

# Not so cool way of writing this Makefile, but works and it's simple
# enough for a dummy to understand it.
fifo:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) $@.ko
clip:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) $@.ko
yoda_superb:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) $@.ko
yoda_works:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) $@.ko
yoda_buggy:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) $@.ko
hello_world:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) $@.ko
hello_irq:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) $@.ko
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

# Some phony rules...
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
install:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules_install
tags:
	make -C /lib/modules/$(shell uname -r)/build tags
