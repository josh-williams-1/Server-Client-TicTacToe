CC = g++

CFLAGS = -g -W

all: server client

server: server.cpp
	$(CC) server.cpp $(CFLAGS) -o Server

client: client.cpp
	$(CC) client.cpp $(CFLAGS) -o Client

clean:
	rm Client Server
