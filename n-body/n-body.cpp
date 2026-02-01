#include "sim_state.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <filesystem>

const double kG = 6.674e-11;
const double epsilon = 0.001;

struct Config {
    std::size_t n;
    double dt;
    std::size_t iterations;
    std::size_t dump_iterations;
    std::string input_file;
    bool use_file = false;
};


nbody::Vec3 compute_force(const nbody::Particle& p1, const nbody::Particle& p2) {
    auto r = p2.pos - p1.pos;
    double r2 = r.x*r.x + r.y*r.y + r.z*r.z + epsilon;
    double inv_r3 = 1.0 / (r2 * std::sqrt(r2));
    return r * (kG * p1.mass * p2.mass * inv_r3);
}

void step(nbody::SimState& sim, std::size_t size, double dt) {
    size_t i = 0;
    for (i = 0; i < size; i++) {
        nbody::Particle& p = sim[i];
        p.force = {0, 0, 0};
    }

    for (i = 0; i < size; i++) {
        nbody::Particle& p = sim[i];
        for (size_t j = i + 1; j < size; j++) {
            
            nbody::Particle& other = sim[j];
            auto F = compute_force(p, other);
            p.force += F;
            other.force -= F;
        }
    }

    for (i = 0; i < size; i++) {
        nbody::Particle& p = sim[i];
        p.vel = p.vel + (p.force / p.mass) * dt;
        p.pos = p.pos + p.vel * dt;
    }
}

int read_input(std::string filename, nbody::SimState& sim) {
    std::ifstream in(filename);
    if (!in) {
        return 1;
    }

    int count;
    in >> count;

    sim.reserve(count);
    int i = 0;
    for (; i < count; i++) {
        nbody::Particle p;

        in >> p.mass
             >> p.pos.x >> p.pos.y >> p.pos.z
             >> p.vel.x >> p.vel.y >> p.vel.z
             >> p.force.x >> p.force.y >> p.force.z;

        if (!in) {
            std::cerr << "Parse error at particle " << i << "\n";
            return 1;
        }

        sim.add_particle(p);
    }

    in.close();
    return 0;
}

int parse_args(int argc, char* argv[], Config& config) {
    if (argc < 5) {
        return 1;
    }

    config.n = std::stoul(argv[1]);
    config.dt = std::stod(argv[2]);
    config.iterations = std::stoul(argv[3]);
    config.dump_iterations = std::stoul(argv[4]);

    if (config.iterations <= 0 || config.dump_iterations <= 0 || config.dt <= 0 || config.n <= 0) {
        return 1;
    }

    if (argc >= 6) {
        config.input_file = argv[5];
        config.use_file = true;
    }

    return 0;
}


int run_sim(std::string out_filename, nbody::SimState& sim, double dt, std::size_t iterations, std::size_t dump_iterations) {
    std::ofstream out(out_filename);
    if (!out) { return 1; }
    sim.to_ofstream(out);
    for (std::size_t i = 0; i < iterations; i++) {
        step(sim, sim.size(), dt);
        if (i % dump_iterations == 0) {
            sim.to_ofstream(out);
        }
    }
    out.close();
    return 0;
}

std::string next_available_output(const std::string& base = "output", const std::string& ext = ".tsv") {
    namespace fs = std::filesystem;

    fs::path p = base + ext;
    if (!fs::exists(p)) {
        return p.string();
    }

    for (std::size_t i = 1; ; ++i) {
        fs::path candidate = base + std::to_string(i) + ext;
        if (!fs::exists(candidate)) {
            return candidate.string();
        }
    }
}


void init_random(nbody::SimState& sim, std::size_t n) {
    std::mt19937_64 rng(42);

    std::uniform_real_distribution<double> pos(-1e11, 1e11);
    std::uniform_real_distribution<double> vel(-3e4, 3e4);
    std::uniform_real_distribution<double> mass(1e5, 1e30);

    sim.reserve(n);

    for (std::size_t i = 0; i < n; i++) {
        nbody::Particle p{};
        p.mass = mass(rng);
        p.pos = {pos(rng), pos(rng), pos(rng)};
        p.vel = {vel(rng), vel(rng), vel(rng)};
        p.force = {0, 0, 0};
        sim.add_particle(p);
    }
}


int main(int argc, char* argv[]) {
    Config config;
    if (parse_args(argc, argv, config) != 0) {
        std::cerr << "Usage:\n" << argv[0] << " N dt iterations dump_iterations [input_file]\nEnter non-zero positive args\n";
        return 1;
    }
    
    nbody::SimState sim;
    if (config.use_file && read_input(config.input_file, sim) != 0) {
        std::cerr << "Failed to read input file\n";
        return 1;
    } else if (!config.use_file) {
        init_random(sim, config.n);
    }

    auto start = std::chrono::steady_clock::now();
    std::string out_filename = next_available_output();
    if (run_sim(out_filename, sim, config.dt, config.iterations, config.dump_iterations) != 0) {
        std::cerr << "Failed to run simulation\n";
        return 1;
    }
    std::cout << "Runtime: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << "μs\n";
    std::cout << "Finished successfully\n";

    return 0;
}