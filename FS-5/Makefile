CC = g++
CFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic
TARGET = manual-append
SRC = manual-append.cpp


.PHONY: all clean
all: $(TARGET)


$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)


clean:
	rm -f $(TARGET)
