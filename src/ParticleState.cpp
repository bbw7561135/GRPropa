#include "grpropa/ParticleState.h"
#include "grpropa/Units.h"
#include "grpropa/Common.h"


#include <HepPID/ParticleIDMethods.hh>

namespace grpropa {

ParticleState::ParticleState(int id, double E, Vector3d pos, Vector3d dir) {
    setId(id);
    setEnergy(E);
    setPosition(pos);
    setDirection(dir);
}

void ParticleState::setPosition(const Vector3d &pos) {
    position = pos;
}

const Vector3d &ParticleState::getPosition() const {
    return position;
}

void ParticleState::setDirection(const Vector3d &dir) {
    direction = dir / dir.getR();
}

const Vector3d &ParticleState::getDirection() const {
    return direction;
}

void ParticleState::setEnergy(double newEnergy) {
    energy = std::max(0., newEnergy); // prevent negative energies
}

double ParticleState::getEnergy() const {
    return energy;
}

void ParticleState::setId(int newId) {
    id = newId;
    charge = HepPID::charge(id) * eplus;
    if (std::abs(id) == 11)
        pmass = mass_electron;
    else
        pmass = 0;
}

int ParticleState::getId() const {
    return id;
}

double ParticleState::getMass() const {
    return pmass;
}

double ParticleState::getCharge() const {
    return charge;
}

double ParticleState::getLorentzFactor() const {
    if (std::abs(id) == 11)
        return energy / (pmass * c_squared);
    else 
        return -1; 
}

void ParticleState::setLorentzFactor(double lf) {
    lf = std::max(0., lf); // prevent negative Lorentz factors
}

Vector3d ParticleState::getVelocity() const {
    return direction * c_light;
}

double ParticleState::getSpeed() const {
    if (id == 22)
        return c_light;
    else 
        return c_light * sqrt(1 - pow(getLorentzFactor(), -2));
}

Vector3d ParticleState::getMomentum() const {
    return direction * (energy / c_light);
}

std::string ParticleState::getDescription() const {
    std::stringstream ss;
    ss << "Particle " << id << ", ";
    ss << "E = " << energy / EeV << " EeV, ";
    ss << "x = " << position / Mpc << " Mpc, ";
    ss << "p = " << direction;
    return ss.str();
}

} // namespace grpropa
