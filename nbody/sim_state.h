#ifndef SIM_STATE_H
#define SIM_STATE_H

#include <vector>
#include <memory>
#include <string>
#include <cmath>
#include <fstream>

namespace nbody {

struct Vec3 {
    double x = 0, y = 0, z = 0;
    Vec3() = default;
    Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator=(const Vec3&) = default;
    Vec3 operator+(const Vec3& o) const { return Vec3(x + o.x, y + o.y, z + o.z); }
    Vec3 operator-(const Vec3& o) const { return Vec3(x - o.x, y - o.y, z - o.z); }
    Vec3 operator*(double s) const { return Vec3(x * s, y * s, z * s); }
    Vec3 operator/(double s) const { return *this * (1/s); }
    double operator!() const { return std::sqrt(x*x + y*y + z*z); }
    void append_to(std::string& out) const { 
        out += std::to_string(x) + "\t" + std::to_string(y) + "\t" + std::to_string(z) + "\t";
    }
};

struct Particle {
    Vec3 pos;
    Vec3 vel;
    Vec3 force;
    double mass;
};

class SimState {
public:
    SimState();
    ~SimState();

    void add_particle(const Particle& p);
    void clear();
    void reserve(std::size_t count);

    std::size_t size() const;
    Particle& operator[](std::size_t i);
    void to_ofstream(std::ofstream& out) const;
private:
    std::vector<Particle> particles_;
    int size_;
};

}

#endif
