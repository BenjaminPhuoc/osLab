CC = gcc
CFLAGS = -Wall -g
TARGET = calc
OBJS = calc.o logic.o
CACHE = .cache

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

calc.o: calc.c calc.h
	$(CC) $(CFLAGS) -c calc.c

logic.o: logic.c calc.h
	$(CC) $(CFLAGS) -c logic.c

clean:
	rm -f $(TARGET) $(OBJS) $(CACHE)