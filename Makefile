PREFIX = /usr/local
CC ?= gcc
LD ?= ld
CFLAGS += -I/usr/local/include/SDL11 -I/usr/local/lib/SDL11 -I/usr/X11R6/include -L/usr/local/lib
LIBS = -lSDL-1.1 -lstdc++ -lc -lSDL_gfx
SOURCES = line_util.cpp manip.cpp render.cpp file_parser.cpp bezier.cpp
MAN = fp.8
PROG = fp
MAINTAINER = Bruce Cran <bruce@cran.org.uk>

all:	fp

fp:  bezier.o line_util.o manip.o render.o file_parser.o
	$(CC) $(CFLAGS) $(LIBS) -o fp file_parser.o bezier.o line_util.o manip.o render.o

file_parser.o:
	$(CC) $(CFLAGS) -c file_parser.cpp

bezier.o:
	$(CC) $(CFLAGS) -c bezier.cpp

line_util.o:
	$(CC) $(CFLAGS) -c line_util.cpp

manip.o:
	$(CC) $(CFLAGS) -c manip.cpp

render.o:
	$(CC) $(CFLAGS) -c render.cpp

install:
	install $(PROG) $(PREFIX)/sbin
	install $(MAN)  $(PREFIX)/man/man8

uninstall:
	rm $(PREFIX)/sbin/$(PROG)
	rm $(PREFIX)/man/man8/$(MAN)

clean: 
	rm -f *.o $(PROG)

cleancompile:
	rm -f *.o
