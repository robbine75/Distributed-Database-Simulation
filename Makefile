CC = g++
CPPFLAGS = -Wall -g
UNIXARGS = -lsocket -lnsl -lresolv

all: server1 server2 server3 client1 client2

server1: dhtserver1.cpp 
	$(CC) $(CPPFLAGS) -o server1 dhtserver1.cpp $(UNIXARGS)

server2: dhtserver2.cpp
	$(CC) $(CPPFLAGS) -o server2 dhtserver2.cpp $(UNIXARGS)

server3: dhtserver3.cpp
	$(CC) $(CPPFLAGS) -o server3 dhtserver3.cpp $(UNIXARGS)

client1: client1.cpp
	$(CC) $(CPPFLAGS) -o client1 client1.cpp $(UNIXARGS)

client2: client2.cpp
	$(CC) $(CPPFLAGS) -o client2 client2.cpp $(UNIXARGS)

clean:
	rm server1 server2 server3 client1 client2 
