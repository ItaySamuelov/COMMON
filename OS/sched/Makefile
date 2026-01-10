# Makefile
CXX      := g++
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra -pedantic
TARGET   := sched_demo
SRC      := sched_demo.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
