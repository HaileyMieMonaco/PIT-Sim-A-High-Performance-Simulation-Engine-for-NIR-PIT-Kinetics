// Implementation of LightTransport class methods for simulating photon transport in a 3D grid using Monte Carlo methods.
// Uses OpenMP for parallelization and Eigen for matrix operations to distribute across multiple CPU cores

#include "core/LightTransport.hpp"
#include <omp.h>
#include <random>
#include <iostream>

LightTransport::LightTransport(int grid_size, double absorption_coeff) 
    : size(grid_size), mu_a(absorption_coeff) {
    // Initialize a 2D slice representation of the 3D tumor volume
    fluence_map = Eigen::MatrixXd::Zero(size, size);
}

void LightTransport::simulate_photons(int num_packets) {
    std::cout << "Simulating " << num_packets << " photons using " 
              << omp_get_max_threads() << " threads..." << std::endl;

    // Parallel region: Each thread simulates a subset of the total packets
    #pragma omp parallel
    {
        // Thread-local random engine to prevent race conditions
        int thread_id = omp_get_thread_num();
        std::mt19937 gen(1337 + thread_id);
        std::uniform_real_distribution<double> dist(0.0, 1.0);

        #pragma omp for
        for (int i = 0; i < num_packets; ++i) {
            // Simplified Monte Carlo Step:
            // 1. Photon enters tissue at (size/2, 0)
            int x = size / 2;
            int y = 0;
            double weight = 1.0;

            // 2. Photon "walks" until weight is absorbed or it leaves the grid
            while (weight > 0.01 && x > 0 && x < size - 1 && y < size - 1) {
                // Absorb a fraction of energy based on the physicochemical properties
                double absorbed = weight * mu_a;
                
                // Critical Section: Protect the shared matrix from simultaneous writes
                #pragma omp atomic
                fluence_map(x, y) += absorbed;

                weight -= absorbed;
                
                // Random scattering step
                x += (dist(gen) > 0.5) ? 1 : -1;
                y += 1;
            }
        }
    }
}