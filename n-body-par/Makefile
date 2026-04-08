CXX      = g++
CXXFLAGS = -O2 -std=c++17 -fopenmp
TARGET   = nbody

$(TARGET): nbody.cpp omp_loop.hpp
	$(CXX) $(CXXFLAGS) -o $@ nbody.cpp

clean:
	rm -f $(TARGET)

run-solar:
	./$(TARGET) planet 200 5000000 10000 4 > solar.tsv

run-rand100:
	./$(TARGET) 100 1 10000 100 4 > rand100.tsv

run-rand1000:
	./$(TARGET) 1000 1 10000 100 4 > rand1000.tsv

.PHONY: clean run-solar run-rand100 run-rand1000
