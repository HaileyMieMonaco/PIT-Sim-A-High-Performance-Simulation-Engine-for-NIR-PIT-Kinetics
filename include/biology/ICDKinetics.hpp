#ifndef ICD_KINETICS_HPP
#define ICD_KINETICS_HPP

#include <vector>

/**
 * @class ICDKinetics
 * @brief Models the release of DAMPs and subsequent immune activation 
 * based on the NIR-PIT review by Monaco et al. (2022).
 */
class ICDKinetics {
public:
    // Constructor taking the initial tumor volume as a parameter
    explicit ICDKinetics(double initial_tumor_vol);

    // Main calculation engine for necrotic marker release [cite: 185]
    void calculate_damp_release(double nir_fluence, double time_hrs);

    // Getter for the probability of Dendritic Cell maturation [cite: 186]
    double get_dc_maturation_probability() const;

    // Getters for current marker concentrations
    double get_atp() const { return current_atp; }
    double get_hmgb1() const { return current_hmgb1; }

private:
    double tumor_volume;
    
    // Kinetic rates for DAMPs unique to NIR-PIT necrosis [cite: 185]
    double release_rate_ATP;   
    double release_rate_HMGB1; 

    // Internal state tracking
    double current_atp = 0.0;
    double current_hmgb1 = 0.0;
};

#endif // ICD_KINETICS_HPP