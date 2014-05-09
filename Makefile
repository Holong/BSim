.EXPORT_ALL_VARIABLES:

TARGET		:= BSim
APPTARGET	:= Application
TOPDIR		:= $(shell /bin/pwd)
SUBDIRS		:= src
MODULEDIR	:= 
APPDIR		:= app
LIBDIR		:= $(TOPDIR)/lib
LIB		:= distorm3

include $(TOPDIR)/config.mk

all : compile
	$(CDOUBLE) $(addsuffix /built-in.o,$(SUBDIRS)) -o $(TARGET) -L$(LIBDIR) -l$(LIB)

include $(TOPDIR)/rules.mk
