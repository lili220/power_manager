FLAGS=$(shell pkg-config --cflags --libs gconf-2.0 gtk+-3.0)
LIBS = -lgconf-2
all: powermanager

powermanager: power_manager.c power_manager.h
	gcc -g -Wall -o power_manager power_manager.c $(FLAGS) $(LIBS)
clean:
	rm -rf power_manager
