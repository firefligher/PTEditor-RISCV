KERNEL ?= $(shell uname -r)
ARCH ?= $(shell uname -m)
obj-m += pteditor.o \
arch/x86/constants.o \
arch/x86/invalidate_tlb_kernel.o \
arch/x86/invalidate_tlb.o \
arch/x86/set_pat.o \
arch/x86/symbols.o \
arch/x86/vm_to_user.o \
shared/shared.o

ccflags-y += -Wno-unused-result

all:
	make -C /lib/modules/${KERNEL}/build M=$(PWD) modules

clean:
	make -C /lib/modules/${KERNEL}/build M=$(PWD) clean
