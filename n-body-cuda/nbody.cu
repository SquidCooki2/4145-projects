#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <vector>
#include <string>
#include <chrono>
#include <cuda_runtime.h>

double G_host = 6.674e-11;

// Constant memory for gravitational constant on device
__device__ __constant__ double G_dev;

struct simulation {
  size_t nbpart;
  
  std::vector<double> mass;

  //position
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> z;

  //velocity
  std::vector<double> vx;
  std::vector<double> vy;
  std::vector<double> vz;

  //force
  std::vector<double> fx;
  std::vector<double> fy;
  std::vector<double> fz;

  
  simulation(size_t nb)
    :nbpart(nb), mass(nb),
     x(nb), y(nb), z(nb),
     vx(nb), vy(nb), vz(nb),
     fx(nb), fy(nb), fz(nb) 
  {}
};

struct device_simulation {
    double *mass;
    double *x, *y, *z;
    double *vx, *vy, *vz;
    double *fx, *fy, *fz;
};

__global__ void reset_force_kernel(device_simulation d_s, size_t nbpart) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < (int)nbpart) {
        d_s.fx[i] = 0.0;
        d_s.fy[i] = 0.0;
        d_s.fz[i] = 0.0;
    }
}

__global__ void update_force_kernel(device_simulation d_s, size_t nbpart) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < (int)nbpart) {
        double softening = 0.1;
        double fx = 0.0;
        double fy = 0.0;
        double fz = 0.0;
        double xi = d_s.x[i];
        double yi = d_s.y[i];
        double zi = d_s.z[i];
        double mi = d_s.mass[i];

        for (size_t j = 0; j < nbpart; ++j) {
            if (i == (int)j) continue;

            double dx = d_s.x[j] - xi;
            double dy = d_s.y[j] - yi;
            double dz = d_s.z[j] - zi;
            double dist_sq = dx * dx + dy * dy + dz * dz;
            double F = G_dev * mi * d_s.mass[j] / (dist_sq + softening);
            double norm = sqrt(dist_sq);
            
            if (norm > 0) {
                fx += (dx / norm) * F;
                fy += (dy / norm) * F;
                fz += (dz / norm) * F;
            }
        }
        d_s.fx[i] = fx;
        d_s.fy[i] = fy;
        d_s.fz[i] = fz;
    }
}

__global__ void update_state_kernel(device_simulation d_s, size_t nbpart, double dt) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < (int)nbpart) {
        // apply force (update velocity)
        d_s.vx[i] += d_s.fx[i] / d_s.mass[i] * dt;
        d_s.vy[i] += d_s.fy[i] / d_s.mass[i] * dt;
        d_s.vz[i] += d_s.fz[i] / d_s.mass[i] * dt;

        // update position
        d_s.x[i] += d_s.vx[i] * dt;
        d_s.y[i] += d_s.vy[i] * dt;
        d_s.z[i] += d_s.vz[i] * dt;
    }
}

void random_init(simulation& s) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dismass(0.9, 1.0);
    std::normal_distribution<double> dispos(0.0, 1.0);
    std::normal_distribution<double> disvel(0.0, 1.0);

    for (size_t i = 0; i < s.nbpart; ++i) {
        s.mass[i] = dismass(gen);
        s.x[i] = dispos(gen);
        s.y[i] = dispos(gen);
        s.z[i] = 0.0;
        s.vx[i] = s.y[i] * 1.5;
        s.vy[i] = -s.x[i] * 1.5;
        s.vz[i] = 0.0;
    }
}

void init_solar(simulation& s) {
    enum Planets {SUN, MERCURY, VENUS, EARTH, MARS, JUPITER, SATURN, URANUS, NEPTUNE, MOON};
    s = simulation(10);

    // Masses in kg
    s.mass[SUN] = 1.9891 * std::pow(10, 30);
    s.mass[MERCURY] = 3.285 * std::pow(10, 23);
    s.mass[VENUS] = 4.867 * std::pow(10, 24);
    s.mass[EARTH] = 5.972 * std::pow(10, 24);
    s.mass[MARS] = 6.39 * std::pow(10, 23);
    s.mass[JUPITER] = 1.898 * std::pow(10, 27);
    s.mass[SATURN] = 5.683 * std::pow(10, 26);
    s.mass[URANUS] = 8.681 * std::pow(10, 25);
    s.mass[NEPTUNE] = 1.024 * std::pow(10, 26);
    s.mass[MOON] = 7.342 * std::pow(10, 22);

    // Positions (in meters) and velocities (in m/s)
    double AU = 1.496 * std::pow(10, 11); // Astronomical Unit

    s.x = {0, 0.39*AU, 0.72*AU, 1.0*AU, 1.52*AU, 5.20*AU, 9.58*AU, 19.22*AU, 30.05*AU, 1.0*AU + 3.844*std::pow(10, 8)};
    s.y = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    s.z = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    s.vx = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    s.vy = {0, 47870, 35020, 29780, 24130, 13070, 9680, 6800, 5430, 29780 + 1022};
    s.vz = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

void load_from_file(simulation& s, std::string filename) {
    std::ifstream in(filename);
    if (!in.is_open()) throw std::runtime_error("Could not open file");
    size_t nbpart;
    in >> nbpart;
    s = simulation(nbpart);
    for (size_t i = 0; i < s.nbpart; ++i) {
        in >> s.mass[i];
        in >> s.x[i] >> s.y[i] >> s.z[i];
        in >> s.vx[i] >> s.vy[i] >> s.vz[i];
        in >> s.fx[i] >> s.fy[i] >> s.fz[i];
    }
    if (!in.good()) throw std::runtime_error("Error reading file");
}

void dump_state(simulation& s) {
    // std::cout << s.nbpart << '\t';
    // for (size_t i = 0; i < s.nbpart; ++i) {
    //     std::cout << s.mass[i] << '\t';
    //     std::cout << s.x[i] << '\t' << s.y[i] << '\t' << s.z[i] << '\t';
    //     std::cout << s.vx[i] << '\t' << s.vy[i] << '\t' << s.vz[i] << '\t';
    //     std::cout << s.fx[i] << '\t' << s.fy[i] << '\t' << s.fz[i] << '\t';
    // }
    // std::cout << '\n';
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "usage: " << argv[0] << " <input> <dt> <nbstep> <printevery> <blocksize>\n";
        return -1;
    }

    double dt = std::atof(argv[2]);
    size_t nbstep = std::atol(argv[3]);
    size_t printevery = std::atol(argv[4]);
    int blocksize = std::atoi(argv[5]);

    simulation s(1);
    std::string inputparam = argv[1];
    if (isdigit(inputparam[0])) {
        size_t nbpart = std::stol(inputparam);
        s = simulation(nbpart);
        random_init(s);
    } else if (inputparam == "planet") {
        init_solar(s);
    } else {
        load_from_file(s, inputparam);
    }

    // CUDA setup
    device_simulation d_s;
    size_t size = s.nbpart * sizeof(double);
    cudaMalloc(&d_s.mass, size);
    cudaMalloc(&d_s.x, size);
    cudaMalloc(&d_s.y, size);
    cudaMalloc(&d_s.z, size);
    cudaMalloc(&d_s.vx, size);
    cudaMalloc(&d_s.vy, size);
    cudaMalloc(&d_s.vz, size);
    cudaMalloc(&d_s.fx, size);
    cudaMalloc(&d_s.fy, size);
    cudaMalloc(&d_s.fz, size);

    cudaMemcpy(d_s.mass, s.mass.data(), size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_s.x, s.x.data(), size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_s.y, s.y.data(), size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_s.z, s.z.data(), size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_s.vx, s.vx.data(), size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_s.vy, s.vy.data(), size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_s.vz, s.vz.data(), size, cudaMemcpyHostToDevice);
    
    cudaMemcpyToSymbol(G_dev, &G_host, sizeof(double));

    int gridsize = (s.nbpart + blocksize - 1) / blocksize;

    cudaEvent_t start_event, stop_event;
    cudaEventCreate(&start_event);
    cudaEventCreate(&stop_event);

    cudaEventRecord(start_event);
    for (size_t step = 0; step < nbstep; step++) {
        if (step % printevery == 0) {
            cudaMemcpy(s.x.data(), d_s.x, size, cudaMemcpyDeviceToHost);
            cudaMemcpy(s.y.data(), d_s.y, size, cudaMemcpyDeviceToHost);
            cudaMemcpy(s.z.data(), d_s.z, size, cudaMemcpyDeviceToHost);
            cudaMemcpy(s.vx.data(), d_s.vx, size, cudaMemcpyDeviceToHost);
            cudaMemcpy(s.vy.data(), d_s.vy, size, cudaMemcpyDeviceToHost);
            cudaMemcpy(s.vz.data(), d_s.vz, size, cudaMemcpyDeviceToHost);
            cudaMemcpy(s.fx.data(), d_s.fx, size, cudaMemcpyDeviceToHost);
            cudaMemcpy(s.fy.data(), d_s.fy, size, cudaMemcpyDeviceToHost);
            cudaMemcpy(s.fz.data(), d_s.fz, size, cudaMemcpyDeviceToHost);
            dump_state(s);
        }

        update_force_kernel<<<gridsize, blocksize>>>(d_s, s.nbpart);
        update_state_kernel<<<gridsize, blocksize>>>(d_s, s.nbpart, dt);
    }
    cudaEventRecord(stop_event);
    cudaEventSynchronize(stop_event);

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start_event, stop_event);
    std::cerr << "Simulation took " << milliseconds / 1000.0 << " seconds" << std::endl;

    // Cleanup
    cudaFree(d_s.mass);
    cudaFree(d_s.x);
    cudaFree(d_s.y);
    cudaFree(d_s.z);
    cudaFree(d_s.vx);
    cudaFree(d_s.vy);
    cudaFree(d_s.vz);
    cudaFree(d_s.fx);
    cudaFree(d_s.fy);
    cudaFree(d_s.fz);

    return 0;
}
