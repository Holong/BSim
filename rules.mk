compile:
	@for dir in $(SUBDIRS) $(MODULEDIR) $(APPDIR); do \
	make -C $$dir || exit $?; \
	done

clean:
	@for dir in $(SUBDIRS) $(MODULEDIR) $(APPDIR); do \
	make -C $$dir clean; \
	done
	rm -rf *.o *.i *.s *.ko $(TARGET) $(APPTARGET)


