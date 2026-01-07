#shit
CC = gcc
CFLAGS = -O2 -D_DEBUG
TARGET = mystd_test
SRCS = mystd.c myallocator.c main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
clean:
	rm -f $(TARGET)
