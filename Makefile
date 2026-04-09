CC = gcc
CFLAGS = -Wall -Wextra

TARGET = tcp_server
SRC = tcp_server.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
