#include "core/LightTransport.hpp"

#include <omp.h>
#include <random>
#include <stdexcept>
#include <string>
#include <iostream>

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
LightTransport::LightTransport(int grid_size, double absorption_coeff)
    : size(grid_size), mu_a(absorption_coeff)
{
    if (size <= 0)
        throw std::invalid_argument(
            "LightTransport: grid_size must be > 0 (got " +
            std::to_string(size) + ")");

    if (mu_a <= 0.0)
        throw std::invalid_argument(
            "LightTransport: absorption_coeff must be > 0 (got " +
            std::to_string(mu_a) + ")");

    fluence_map = Eigen::MatrixXd::Zero(size, size);
}

// -----------------------------------------------------------------------------
// simulate_photons()
// -----------------------------------------------------------------------------
void LightTransport::simulate_photons(int num_packets)
{
    if (num_packets <= 0)
        throw std::invalid_argument(
            "LightTransport::simulate_photons: num_packets must be > 0 (got " +
            std::to_string(num_packets) + ")");

    std::cout << "Simulating " << num_packets << " photon packets across "
              << omp_get_max_threads() << " thread(s)...\n";

    // Base seed is fixed for reproducibility; each thread offsets it so
    // their sequences are independent (avoiding correlated random walks).
    const unsigned int BASE_SEED = 42u;

    #pragma omp parallel
    {
        const int thread_id = omp_get_thread_num();
        std::mt19937 rng(BASE_SEED + static_cast<unsigned int>(thread_id));
        std::uniform_real_distribution<double> uniform(0.0, 1.0);

        #pragma omp for schedule(dynamic, 64)
        for (int i = 0; i < num_packets; ++i) {
            // Photon enters at the beam centre on the top surface.
            int    x      = size / 2;
            int    y      = 0;
            double weight = 1.0;

            // Propagate until the packet is exhausted or exits the grid.
            while (weight > 1e-4 && x > 0 && x < size - 1 && y < size - 1) {
                const double absorbed = weight * mu_a;

                // Atomic update prevents data races on the shared matrix.
                #pragma omp atomic
                fluence_map(x, y) += absorbed;

                weight -= absorbed;

                // Isotropic lateral scattering + downward bias (beam direction).
                x += (uniform(rng) > 0.5) ? 1 : -1;
                y += 1;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Aggregate statistics
// -----------------------------------------------------------------------------
double LightTransport::mean_fluence() const
{
    return fluence_map.mean();
}

double LightTransport::peak_fluence() const
{
    return fluence_map.maxCoeff();
}
