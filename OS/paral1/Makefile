CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -pthread
TARGET = rw_test

OBJS = main.o rw.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp rw.h
	$(CXX) $(CXXFLAGS) -c main.cpp

rw.o: rw.cpp rw.h
	$(CXX) $(CXXFLAGS) -c rw.cpp

clean:
	rm -f *.o $(TARGET)