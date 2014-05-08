.EXPORT_ALL_VARIABLES:

TARGET		:= BSim
APPTARGET	:= application
TOPDIR		:= $(shell /bin/pwd)
SUBDIRS		:= src
MODULEDIR	:= module
APPDIR		:= app

include $(TOPDIR)/config.mk

all : compile
	$(CDOUBLE) $(addsuffix /built-in.o,$(SUBDIRS)) -o $(TARGET)

include $(TOPDIR)/rules.mk
