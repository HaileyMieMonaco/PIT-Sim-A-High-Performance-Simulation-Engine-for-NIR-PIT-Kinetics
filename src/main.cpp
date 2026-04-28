#include <iostream>
#include <iomanip>
#include <fstream>  // Added for file handling
#include "biology/ICDKinetics.hpp"
#include "core/LightTransport.hpp"

int main() {
    double tumor_volume = 500.0;
    double nir_fluence = 40.0;
    int simulation_hours = 48;

    ICDKinetics biology(tumor_volume);
    
    // Create a CSV file in the data directory
    std::ofstream outFile("../data/simulation_results.csv");
    outFile << "Hour,ATP,HMGB1,DC_Prob\n"; // Header

    std::cout << "--- Running Simulation and Exporting Data ---" << std::endl;

    for (int t = 0; t <= simulation_hours; t += 2) { // Smaller steps for a smoother plot
        biology.calculate_damp_release(nir_fluence, static_cast<double>(t));
        
        // Write to CSV
        outFile << t << "," << biology.get_atp() << "," 
                << biology.get_hmgb1() << "," << biology.get_dc_maturation_probability() << "\n";
    }

    outFile.close();
    std::cout << "Results saved to data/simulation_results.csv" << std::endl;
    return 0;
}