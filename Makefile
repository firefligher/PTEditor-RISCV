all: ptedit_header.h module example header test

deb:
	dpkg-buildpackage

demos: ptedit_header.h
	$(MAKE) -C $@

module:
	$(MAKE) -C $@

test: ptedit_header.h
	$(MAKE) -C $@

.PHONY: all module demos tests

ptedit_header.h: config.h ptedit_arm.h ptedit_riscv.h ptedit_x86.h ptedit.c \
ptedit.h types.h
	php build_header.php ptedit.c > $@

example: example.c ptedit_header.h
	gcc -Wall -Wextra example.c -g -o example

clean:
	$(MAKE) -C module clean
	$(MAKE) -C demos clean
	$(MAKE) -C test clean
	rm -f example *.o
