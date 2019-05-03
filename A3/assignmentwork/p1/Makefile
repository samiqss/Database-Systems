#
## CSCI 370 : Sample Makefile for OCCI compilation
#

CFLAGS = -Wall -Wextra -I/usr/local/lib/Oracle/instantclient_11_2/sdk/include
LFLAGS = -L/usr/local/lib/Oracle/instantclient_11_2 -locci -lociei
all: main

main.o: main.cpp
	g++-4.9 -c $(CFLAGS) main.cpp

main: main.o
	g++-4.9 $(LFLAGS) -o main main.o

clean:
	rm main.o main

