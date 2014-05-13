.EXPORT_ALL_VARIABLES:

TARGET		:= BSim
APPTARGET	:= Application
TOPDIR		:= $(shell /bin/pwd)
SUBDIRS		:= src
MODULEDIR	:= 
APPDIR		:= app
LIBDIR		:= $(TOPDIR)/lib
LIBNAME		:= distorm3 error
LIBS		:= $(foreach name, $(LIBNAME), -l$(name))

include $(TOPDIR)/config.mk

all : compile
	$(CDOUBLE) $(addsuffix /built-in.o,$(SUBDIRS)) -o $(TARGET) -L$(LIBDIR) $(LIBS)

include $(TOPDIR)/rules.mk
