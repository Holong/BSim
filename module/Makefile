obj-m := khello.o

KDIR := /home/moon/odroid/linux

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules
	cp $(patsubst %.o, %.ko, $(obj-m)) ../

clean:
	rm -f *.symvers
	rm -f *.ko
	rm -f *.o
	rm -f *.mod.c
	rm -f *.order
	rm -f .*.cmd
	rm -rf .tmp_versions
