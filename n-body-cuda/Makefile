CXXFLAGS=-O3

all: nbody nbody_gpu

nbody: nbody.cpp
	g++ -O3 nbody.cpp -o nbody

nbody_gpu: nbody.cu
	nvcc -O3 -arch=sm_61 nbody.cu -o nbody_gpu

solar.out: nbody
	date
	./nbody planet 200 5000000 10000 > solar.out # maybe a minutes
	date

solar.pdf: solar.out
	python3 plot.py solar.out solar.pdf 1000 

random.out: nbody
	date
	./nbody 1000 1 10000 100 > random.out # maybe 5 minutes
	date

clean:
	rm -f nbody nbody_gpu solar.out random.out
