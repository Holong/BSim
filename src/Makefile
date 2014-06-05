ifndef TOPDIR
TOPDIR := ..
endif

include $(TOPDIR)/config.mk

TARGET		:= built-in.o
SUBDIRS		:= predictor
OBJS		:= $(patsubst %.S, %.o, $(wildcard *.S))
OBJS		+= $(patsubst %.c, %.o, $(wildcard *.c))
OBJS		+= $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all : compile $(OBJS)
	$(LD) -r $(addsuffix /$(TARGET),$(SUBDIRS)) $(OBJS) -o $(TARGET)

compile:
	@for dir in $(SUBDIRS); do \
	make -C $$dir || exit $?; \
	done

clean:
	@for dir in $(SUBDIRS); do \
	make -C $$dir clean; \
	done
	rm -rf *.o *.i *.s $(TARGET)


