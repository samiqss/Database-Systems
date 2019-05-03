#
## CSCI 370 : Sample Makefile for OCCI compilation
#

CFLAGS = -Wall -Wextra -I/usr/local/lib/Oracle/instantclient_11_2/sdk/include
LFLAGS = -L/usr/local/lib/Oracle/instantclient_11_2 -locci -lociei
all: p1 p2 p3

p1.o: p1.cpp
	g++-4.9 -c $(CFLAGS) p1.cpp

p1: p1.o
	g++-4.9 $(LFLAGS) -o p1 p1.o

p2.o: p2.cpp
	g++-4.9 -c $(CFLAGS) p2.cpp

p2: p2.o
	g++-4.9 $(LFLAGS) -o p2 p2.o

p3.o: p3.cpp
	g++-4.9 -c $(CFLAGS) p3.cpp

p3: p3.o
	g++-4.9 $(LFLAGS) -o p3 p3.o

clean:
	rm *.o p1 p2 p3
