ifneq ($(KERNELRELEASE),)
	obj-m := antiarp.o
else
	BUILD ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
	EXTRA_CFLAGS += -DDEBUG

all:
	make -C $(BUILD) M=$(PWD) modules

.PHONY clean:
	make -C $(BUILD) M=$(PWD) clean
endif
