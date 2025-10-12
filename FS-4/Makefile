CC = g++
CFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic
TARGET = redirect-cin
SRC = redirect-cin.cpp

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
