#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <stdexcept>

#include "core/LightTransport.hpp"
#include "biology/ICDKinetics.hpp"

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------
static constexpr int    GRID_SIZE       = 50;   // voxels per side
static constexpr double MU_A            = 0.10; // absorption coeff (cm⁻¹)
static constexpr int    PHOTON_PACKETS  = 100'000;
static constexpr double TUMOR_VOLUME    = 500.0; // mm³
static constexpr int    SIM_HOURS       = 48;
static constexpr int    TIME_STEP_HRS   = 2;
static constexpr char   OUTPUT_CSV[]    = "../data/simulation_results.csv";

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    std::cout << "=================================================\n";
    std::cout << "  PIT-Sim: NIR-PIT Kinetics Simulation Engine\n";
    std::cout << "  Based on Monaco et al. (2022)\n";
    std::cout << "=================================================\n\n";

    // ------------------------------------------------------------------
    // Stage 1: Light Transport
    //   Simulate photon diffusion to compute the mean fluence actually
    //   delivered to the tumor volume.  This feeds directly into the
    //   biology model so the two modules are physically coupled.
    // ------------------------------------------------------------------
    std::cout << "--- Stage 1: Monte Carlo Light Transport ---\n";

    LightTransport light(GRID_SIZE, MU_A);
    light.simulate_photons(PHOTON_PACKETS);

    const double mean_fluence = light.mean_fluence();
    const double peak_fluence = light.peak_fluence();

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  Mean fluence  : " << mean_fluence << " J/cm²\n";
    std::cout << "  Peak fluence  : " << peak_fluence << " J/cm²\n\n";

    // ------------------------------------------------------------------
    // Stage 2: ICD Biology
    //   Use the *computed* mean fluence (not a hard-coded constant) so
    //   that varying light transport parameters propagates through the
    //   full simulation pipeline.
    // ------------------------------------------------------------------
    std::cout << "--- Stage 2: Immunogenic Cell Death Kinetics ---\n";

    ICDKinetics biology(TUMOR_VOLUME, mean_fluence);

    // Ensure the output directory exists before writing.
    try {
        std::filesystem::create_directories("../data");
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Warning: could not create data directory: " << e.what() << "\n";
    }

    std::ofstream csv(OUTPUT_CSV);
    if (!csv.is_open()) {
        std::cerr << "Error: cannot open " << OUTPUT_CSV << " for writing.\n";
        return 1;
    }

    csv << "Hour,CRT,ATP,HMGB1,DC_Maturation_Prob\n";

    // Column header
    std::cout << "\n"
              << std::setw(6)  << "Hour"
              << std::setw(12) << "CRT"
              << std::setw(12) << "ATP"
              << std::setw(12) << "HMGB1"
              << std::setw(16) << "DC_Mat_Prob"
              << "\n"
              << std::string(58, '-') << "\n";

    for (int t = 0; t <= SIM_HOURS; t += TIME_STEP_HRS) {
        biology.update(static_cast<double>(t));

        const double crt   = biology.get_crt();
        const double atp   = biology.get_atp();
        const double hmgb1 = biology.get_hmgb1();
        const double dc_p  = biology.get_dc_maturation_probability();

        csv  << t     << ","
             << crt   << ","
             << atp   << ","
             << hmgb1 << ","
             << dc_p  << "\n";

        std::cout << std::setw(6)  << t
                  << std::setw(12) << std::setprecision(2) << crt
                  << std::setw(12) << atp
                  << std::setw(12) << hmgb1
                  << std::setw(16) << std::setprecision(4) << dc_p
                  << "\n";
    }

    csv.close();

    std::cout << "\nResults written to " << OUTPUT_CSV << "\n";
    std::cout << "Run scripts/plot_results.py to generate figures.\n";

    return 0;
}
