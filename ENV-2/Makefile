CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra


SRCS = main.cpp Complex.cpp sort_complex.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = env2


all: $(TARGET)


$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)


%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: all clean


clean:
	rm -f $(OBJS) $(TARGET)
