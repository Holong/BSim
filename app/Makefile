CC 		:= gcc
CFLAGS 		:= -W -Wall -g
OBJECT 		:= $(patsubst %.c, %.o, $(wildcard *.c))

ifdef	APPTARGET
TARGET		:= $(APPTARGET)
else
TARGET		:= test
endif

.SUFFIXES : .o .c
%.o : %.c
	$(CC) $(CFLAGS) -c $<

all : $(TARGET)
	cp $(TARGET) ../

$(TARGET) : $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $^


clean :
	rm $(OBJECT) -rf
	rm $(TARGET) -rf
