CXX = g++
CXXFLAGS = -O3 -std=c++17 -fopenmp

all: merge_sort

merge_sort: merge_sort_par.cpp omp_tasking.hpp
	$(CXX) $(CXXFLAGS) merge_sort_par.cpp -o merge_sort

clean:
	rm -f merge_sort