CFLAGS= -pthread -fcommon
CC=gcc $(CFLAGS)

all: p1

p1: CFLAGS += -DWORK_THREAD
p1: main

p2: main

main: bkwrk bktask bktpool
	$(CC) -c -o main.o main.c
	$(CC) -o mypool main.o bkwrk.o bktask.o bktpool.o

bkwrk: bkwrk.c bktpool.h
	$(CC) -c -o bkwrk.o bkwrk.c

bktask: bktask.c bktpool.h
	$(CC) -c -o bktask.o bktask.c 

bktpool: bktpool.c bktpool.h
	$(CC) -c -o bktpool.o bktpool.c

clean:
	rm *.o
	rm mypool
