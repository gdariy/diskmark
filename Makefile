CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic
LDFLAGS = -Wl,-Bstatic -lncurses -ltinfo -Wl,-Bdynamic

SOURCES = benchmark.c main.c findmounts.c

TARGET = diskmark

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o
