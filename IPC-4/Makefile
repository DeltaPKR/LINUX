CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDLIBS   := -pthread

TARGETS := server client

all: $(TARGETS)

server: server.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

client: client.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: all clean run-server run-client

clean:
	rm -f $(TARGETS) *.o

run-server: server
	./server

run-client: client
	./client
