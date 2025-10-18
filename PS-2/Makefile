CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall
TARGET := interactive-shell

all: $(TARGET)

$(TARGET): interactive-shell.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
