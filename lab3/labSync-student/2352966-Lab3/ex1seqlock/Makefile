CC = gcc
CFLAGS = -Wall -std=c11 -pthread
TARGET = seqlock
SRCS = seqlock.c

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
