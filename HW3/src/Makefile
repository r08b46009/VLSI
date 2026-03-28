CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall -Wextra -pedantic
TARGET = ../bin/hw3
SOURCES = main.cpp SA.cpp

all: $(TARGET)

$(TARGET): $(SOURCES) SA.hpp
	@mkdir -p ../bin
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
