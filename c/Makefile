# gcc -Wall -I/usr/include/libxml2 -o ex1 ex1.c -lxml2
CC = gcc
CFLAGS = -g -Wall
INCLUDES = -I/usr/include/libxml2
MAIN = xmlrdr
LIBS = -lxml2
SRCS = reader1.c
OBJS = $(SRCS:.c=.o)

.PHONY: depend clean

all: $(MAIN)

$(MAIN): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OBJS) $(SRCS) $(LIBS)

clean:
	$(RM) *.o *~ $(MAIN)
