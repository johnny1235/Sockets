CC=g++
CFLAGS= -Wall -std=c++14
SERVER = contactServer
NODE = P2Pnode

all: $(SERVER).cpp $(NODE).cpp
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER).cpp
	$(CC) $(CFLAGS) -o $(NODE) $(NODE).cpp

server: $(SERVER).cpp
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER).cpp

node: $(NODE).cpp
	$(CC) $(CFLAGS) -o $(NODE) $(NODE).cpp

clean:
	$(RM) $(SERVER) $(NODE)
