all: ptedit_header.h module example test

deb:
	dpkg-buildpackage

demos: ptedit_header.h
	cd $@ && make

module:
	cd $@ && make

test: ptedit_header.h
	cd $@ && make

.PHONY: all module demos test

ptedit_header.h: config.h ptedit_arm.h ptedit_riscv.h ptedit_x86.h ptedit.c \
ptedit.h types.h
	php build_header.php ptedit.c > $@

example: example.c ptedit_header.h
	gcc -Wall -Wextra example.c -g -o example

clean:
	$(MAKE) -C module clean
	$(MAKE) -C demos clean
	$(MAKE) -C test clean
	rm -f example *.o ptedit_heaer.h
