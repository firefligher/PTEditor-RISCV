KERNEL ?= $(shell uname -r)
ARCH ?= $(shell uname -m)
obj-m += pteditor.o

pteditor-objs := $(PWD)/pteditor_module.o
pteditor-objs += $(patsubst %.c,%.o,$(wildcard $(PWD)/arch/$(ARCH)/*.c))
pteditor-objs += $(patsubst %.c,%.o,$(wildcard $(PWD)/command/*.c))
pteditor-objs += $(patsubst %.c,%.o,$(wildcard $(PWD)/memory/*.c))
pteditor-objs += $(patsubst %.c,%.o,$(wildcard $(PWD)/shared/*.c))
pteditor-objs += $(patsubst %.c,%.o,$(wildcard $(PWD)/userland/*.c))
pteditor-objs := $(pteditor-objs:$(PWD)/%=%)

ccflags-y += -Wno-unused-result
ccflags-y += -DPTEDIT_MODULE_BUILD=1

ifdef PTEDIT_TINA_BUILD
	ccflags-y += -DPTEDIT_TINA_BUILD=1
endif

all:
	echo "$(pteditor-objs)"
	make -C /lib/modules/${KERNEL}/build M=$(PWD) modules

clean:
	make -C /lib/modules/${KERNEL}/build M=$(PWD) clean
