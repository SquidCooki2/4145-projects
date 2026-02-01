#include "sim_state.h"
#include <string>
#include <fstream>

namespace nbody {

SimState::SimState() = default;
SimState::~SimState() = default;

std::size_t SimState::size() const {
    return particles_.size();
}

Particle& SimState::operator[](std::size_t i) {
    return particles_[i];
}

void SimState::add_particle(const Particle& p) {
    particles_.push_back(p);
}

void SimState::clear() {
    particles_.clear();
}

void SimState::reserve(std::size_t count) {
    particles_.reserve(count);
}

void SimState::to_ofstream(std::ofstream& out) const {
    out << size() << '\t';
    for (nbody::Particle p : particles_) {
        out << p.mass << '\t'
            << p.pos.x << '\t' << p.pos.y << '\t' << p.pos.z << '\t'
            << p.vel.x << '\t' << p.vel.y << '\t' << p.vel.z << '\t'
            << p.force.x << '\t' << p.force.y << '\t' << p.force.z << '\t';
    }
    out << '\n';
}



}