CC = g++
CFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic
TARGET = hacker-rm
SRC = hacker-rm.cpp

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
