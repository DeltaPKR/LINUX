CC = g++
CFLAGS = -std=c++17 -O2 -Wall -Wextra -pthread
LDFLAGS = -pthread
TARGET = array-summary
SRCS = array-summary.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
