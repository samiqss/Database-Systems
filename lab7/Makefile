#
## CSCI 370: Sample Makefile for OCCI compilation
#

G = g++-4.9
CFLAGS = -I/usr/local/lib/Oracle/instantclient_11_2/sdk/include
LFLAGS = -L/usr/local/lib/Oracle/instantclient_11_2 -locci -lociei
all: exampleP exampleU

lab8: soln8.cpp
	$(G) $(CFLAGS) $(LFLAGS) soln8.cpp -o lab8

lab7: soln7.cpp
	$(G) $(CFLAGS) $(LFLAGS) soln7.cpp -o lab7

examplePrepared.o: examplePrepared.cpp
	$(G) -c $(CFLAGS) examplePrepared.cpp

exampleP: examplePrepared.o
	$(G) $(LFLAGS) -o exampleP examplePrepared.o

exampleUnprepared.o: exampleUnprepared.cpp
	$(G) -c $(CFLAGS) exampleUnprepared.cpp

exampleU: exampleUnprepared.o
	$(G) $(LFLAGS) -o exampleU exampleUnprepared.o

clean:
	rm -f *.o exampleP exampleU


