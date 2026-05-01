CXX = g++
CXXFLAGS =  -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format
LDFLAGS = -lm -pthread

TARGET = a.out
SOURCES = main.cpp Solver.cpp Time.cpp reduce_sum.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = header.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Правила компиляции
main.o: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c main.cpp

Solver.o: Solver.cpp 
	$(CXX) $(CXXFLAGS) -c Solver.cpp
Time.o: Time.cpp 
	$(CXX) $(CXXFLAGS) -c Time.cpp
reduce_sum.o: reduce_sum.cpp
	$(CXX) $(CXXFLAGS) -c reduce_sum.cpp



# Утилиты
clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all

debug: CXXFLAGS += -g -DDEBUG
debug: rebuild

