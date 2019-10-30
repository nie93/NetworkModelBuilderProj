CC = g++

CFLAGS = -g -Wall
INCLUDES = -I/usr/include/libxml2
MAIN = xmlrdr
LIBS = -lxml2
SRCS = tree1.cc
OBJS = $(SRCS:.cc=.o)

.PHONY: depend clean

all: $(MAIN)

$(MAIN): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OBJS) $(SRCS) $(LIBS)

d:
	gdb $(OBJS) 

r:
	./$(OBJS)

clean:
	$(RM) *.o *~ $(MAIN)
