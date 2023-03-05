KERNEL ?= $(shell uname -r)
ARCH ?= $(shell uname -m)
obj-m += pteditor.o

pteditor-objs := $(PWD)/pteditor_module.o
pteditor-objs += $(patsubst %.c,%.o,$(wildcard $(PWD)/arch/$(ARCH)/*.c))
pteditor-objs += $(patsubst %.c,%.o,$(wildcard $(PWD)/shared/*.c))
pteditor-objs += $(patsubst %.c,%.o,$(wildcard $(PWD)/userland/*.c))
pteditor-objs := $(pteditor-objs:$(PWD)/%=%)

#pteditor-objs := pteditor_module.o
#pteditor-objs += $(shell find arch/$(ARCH) -type f)

#$(patsubst %.c,%.o,$(wildcard shared/*.c))

ccflags-y += -Wno-unused-result

all:
	echo "$(pteditor-objs)"
	make -C /lib/modules/${KERNEL}/build M=$(PWD) modules

clean:
	make -C /lib/modules/${KERNEL}/build M=$(PWD) clean
