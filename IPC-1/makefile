CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17
TARGET = handler

all: $(TARGET)

$(TARGET): handler.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) handler.o

handler.o: handler.cpp
	$(CXX) $(CXXFLAGS) -c handler.cpp

clean:
	rm -f handler.o handler
