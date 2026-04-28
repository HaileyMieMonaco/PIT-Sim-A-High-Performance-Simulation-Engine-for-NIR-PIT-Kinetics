#include "biology/ICDKinetics.hpp"
#include <cmath>
#include <algorithm>

// Constructor: Initializes the biological constants for ICD
ICDKinetics::ICDKinetics(double initial_tumor_vol) 
    : tumor_volume(initial_tumor_vol) {
    // Rates based on preclinical observations of rapid necrosis
    release_rate_ATP = 0.85;   // Rapid release upon membrane rupture
    release_rate_HMGB1 = 0.45; // Slower nuclear protein release
}

/**
 * Calculates the concentration of DAMPs following NIR irradiation.
 * Based on the physicochemical changes in the APC complex[cite: 175].
 */
void ICDKinetics::calculate_damp_release(double nir_fluence, double time_hrs) {
    // Stress response is dose-dependent on NIR light irradiation [cite: 211]
    double death_fraction = 1.0 - std::exp(-nir_fluence * 0.1);
    
    // Release kinetics following a sigmoidal saturation curve
    // ATP release occurs almost immediately after cell bursting [cite: 185]
    current_atp = tumor_volume * death_fraction * (1.0 - std::exp(-release_rate_ATP * time_hrs));
    
    // HMGB1 release is a slightly more durable effect [cite: 15]
    current_hmgb1 = tumor_volume * death_fraction * (1.0 - std::exp(-release_rate_HMGB1 * time_hrs));
}

/**
 * Models the probability of DC maturation based on DAMP concentration.
 * Mature DCs are the bridge to CD8+ T cell priming[cite: 186].
 */
double ICDKinetics::get_dc_maturation_probability() const {
    // Combine DAMP signals to determine the strength of the immune "alert"
    double signal_strength = current_atp + current_hmgb1;
    return signal_strength / (signal_strength + 10.0); // Hill equation for saturation
}