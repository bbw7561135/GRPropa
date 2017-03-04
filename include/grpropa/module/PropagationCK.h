#ifndef GRPROPA_PROPAGATIONCK_H
#define GRPROPA_PROPAGATIONCK_H

#include "grpropa/Module.h"
#include "grpropa/Units.h"
#include "grpropa/magneticField/MagneticField.h"

namespace grpropa {

/**
 @class PropagationCK
 @brief Propagation through magnetic fields using the Cash-Karp method.

 This module solves the equations of motion of a relativistic charged particle when propagating through a magnetic field.\n
 It uses the Runge-Kutta integration method with Cash-Karp coefficients.\n
 The step size control tries to keep the relative error close to, but smaller than the designated tolerance.
 Additionally a minimum and maximum size for the steps can be set.
 For neutral particles a rectilinear propagation is applied and a next step of the maximum step size proposed.
 */
class PropagationCK: public Module {
public:
    class Y {
    public:
        Vector3d x, u; /*< phase-point: position and direction */

        Y() {
        }

        Y(const Vector3d &x, const Vector3d &u) :
                x(x), u(u) {
        }

        Y(double f) :
                x(Vector3d(f, f, f)), u(Vector3d(f, f, f)) {
        }

        Y operator *(double f) const {
            return Y(x * f, u * f);
        }

        Y &operator +=(const Y &y) {
            x += y.x;
            u += y.u;
            return *this;
        }
    };

private:
    std::vector<double> a, b, bs; /*< Cash-Karp coefficients */
    ref_ptr<MagneticField> field;
    double tolerance; /*< target relative error of the numerical integration */
    double minStep; /*< minimum step size of the propagation */
    double maxStep; /*< maximum step size of the propagation */
    int nMaxIterations;

public:
    PropagationCK(ref_ptr<MagneticField> field = NULL, double tolerance = 1e-3, double minStep = 0.1 * kpc, double maxStep = 1 * Mpc, int nMaxIterations = 10000);
    void process(Candidate *candidate) const;

    // derivative of phase point, dY/dt = d/dt(x, u) = (v, du/dt)
    // du/dt = q*c^2/E * (u x B)
    Y dYdt(const Y &y, ParticleState &p) const;

    void tryStep(const Y &y, Y &out, Y &error, double t, ParticleState &p) const;

    void setField(ref_ptr<MagneticField> field);
    void setTolerance(double tolerance);
    void setMinimumStep(double minStep);
    void setMaximumStep(double maxStep);

    double getTolerance() const;
    double getMinimumStep() const;
    double getMaximumStep() const;
    std::string getDescription() const;
};

} // namespace grpropa

#endif // GRPROPA_PROPAGATIONCK_H
