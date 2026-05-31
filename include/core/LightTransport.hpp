#ifndef LIGHT_TRANSPORT_HPP
#define LIGHT_TRANSPORT_HPP

#include <Eigen/Dense>

/**
 * @class LightTransport
 * @brief Monte Carlo simulation of 690 nm photon diffusion through a
 *        voxelized tumor volume.
 *
 * NIR-PIT uses 690 nm light to activate the IR700 photosensitizer conjugated
 * to a targeting antibody. Delivering an effective fluence to deep-tissue
 * tumors is a key clinical challenge (Monaco et al. 2022, §2.1).
 *
 * This module models a 2-D cross-section of the tumor using a simplified
 * random-walk photon propagation scheme, parallelized with OpenMP.
 * Each photon packet carries a fractional weight that decays according to
 * the Beer–Lambert absorption coefficient (mu_a).
 */
class LightTransport {
public:
    /**
     * @param grid_size        Number of voxels per side (must be > 0)
     * @param absorption_coeff Absorption coefficient mu_a (cm⁻¹, must be > 0)
     * @throws std::invalid_argument if parameters are out of range
     */
    LightTransport(int grid_size, double absorption_coeff);

    /**
     * @brief Run the Monte Carlo simulation.
     *
     * Photons enter at the surface (x = grid_size/2, y = 0) and perform a
     * biased random walk, depositing absorbed energy in the fluence map.
     * Each OpenMP thread uses an independently seeded Mersenne Twister to
     * avoid race conditions while maintaining reproducibility for a fixed
     * number of threads.
     *
     * @param num_packets Number of photon packets to simulate (must be > 0)
     */
    void simulate_photons(int num_packets);

    /// Mean fluence (J/cm²) averaged over all grid voxels
    double mean_fluence() const;

    /// Peak fluence at the entry point of the photon beam
    double peak_fluence() const;

    /// Read-only access to the full 2-D fluence map
    const Eigen::MatrixXd& get_fluence_map() const { return fluence_map; }

private:
    int    size; ///< Grid dimension
    double mu_a; ///< Absorption coefficient (cm⁻¹)
    Eigen::MatrixXd fluence_map;
};

#endif // LIGHT_TRANSPORT_HPP
