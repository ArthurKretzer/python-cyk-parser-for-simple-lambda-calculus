CXX = g++
CXXFLAGS = -std=c++11 -Wall

TARGET = main

.PHONY: all clean run

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

run: $(TARGET)
	@echo $(TARGET)
	./$(TARGET) < sample_input.txt