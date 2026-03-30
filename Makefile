# Makefile for HW2 (C++17)
# Usage:
#   make
#   make run
#   make clean
#
# If you saved your code as hw2.cpp, this works as-is.
# If your file name is different, change SRC below.

CXX      := g++
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra -pedantic
LDFLAGS  :=
TARGET   := hw2
SRC      := hw2.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) result
