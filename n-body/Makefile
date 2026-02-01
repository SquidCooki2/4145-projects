CXX      := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -pedantic
TARGET   := n-body
OBJS 	 := n-body.o sim_state.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

n-body.o: n-body.cpp sim_state.h
	$(CXX) $(CXXFLAGS) -c n-body.cpp

sim_state.o: sim_state.cpp sim_state.h
	$(CXX) $(CXXFLAGS) -c sim_state.cpp

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
