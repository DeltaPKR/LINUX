CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -O2

SRC_SERVER = Socket.cpp ThreadPool.cpp TcpServer.cpp server.cpp
SRC_CLIENT = Socket.cpp client.cpp

all: server client

server: $(SRC_SERVER)
	$(CXX) $(CXXFLAGS) $^ -o $@

client: $(SRC_CLIENT)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f server client

