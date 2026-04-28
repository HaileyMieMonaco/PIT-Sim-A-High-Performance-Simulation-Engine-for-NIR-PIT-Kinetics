// Header file for LightTransport class, which simulates photon transport in a 3D grid using Monte Carlo methods.

#ifndef LIGHT_TRANSPORT_HPP
#define LIGHT_TRANSPORT_HPP

#include <vector>
#include <Eigen/Dense>

/**
 * @class LightTransport
 * @brief Simulates 690nm photon diffusion in 3D tumor volumes using Monte Carlo methods.
 * Referenced from Monaco et al. (2022) regarding deep-tissue penetration challenges.
 */
class LightTransport {
public:
    LightTransport(int grid_size, double absorption_coeff);

    // Parallelized simulation of N photon packets using OpenMP
    void simulate_photons(int num_packets);

    // Returns the calculated light intensity (fluence) across the tumor grid
    const Eigen::MatrixXd& get_fluence_map() const { return fluence_map; }

private:
    int size;
    double mu_a; // Absorption coefficient
    Eigen::MatrixXd fluence_map;
};

#endif // LIGHT_TRANSPORT_HPP