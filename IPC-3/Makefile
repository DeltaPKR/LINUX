CXX := g++
CXXFLAGS := -std=gnu++17 -O2 -Wall -Wextra
SRCS := shared_array.cpp
OBJS := $(SRCS:.cpp=.o)

all: first second

shared_array.o: shared_array.cpp shared_array.h
	$(CXX) $(CXXFLAGS) -c shared_array.cpp -o shared_array.o

first: shared_array.o first.cpp
	$(CXX) $(CXXFLAGS) first.cpp shared_array.o -o first -pthread

second: shared_array.o second.cpp
	$(CXX) $(CXXFLAGS) second.cpp shared_array.o -o second -pthread

clean:
	rm -f *.o first second

