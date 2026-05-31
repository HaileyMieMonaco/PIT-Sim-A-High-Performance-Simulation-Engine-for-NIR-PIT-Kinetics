#include "biology/ICDKinetics.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
ICDKinetics::ICDKinetics(double initial_tumor_vol, double effective_fluence)
    : tumor_volume(initial_tumor_vol),
      nir_fluence(effective_fluence)
{
    if (tumor_volume <= 0.0)
        throw std::invalid_argument(
            "ICDKinetics: tumor_volume must be > 0 (got " +
            std::to_string(tumor_volume) + ")");

    if (nir_fluence < 0.0)
        throw std::invalid_argument(
            "ICDKinetics: effective_fluence must be >= 0 (got " +
            std::to_string(nir_fluence) + ")");

    // Pre-compute the dose-dependent death fraction once (Beer-Lambert).
    // Coefficient 0.1 (cm²/J) reflects the effective optical cross-section
    // of the IR700-antibody conjugate at 690 nm (Monaco et al. 2022, §2.3).
    death_fraction = 1.0 - std::exp(-nir_fluence * 0.1);
}

// -----------------------------------------------------------------------------
// update()  — advance DAMP concentrations to time_hrs post-irradiation
// -----------------------------------------------------------------------------
void ICDKinetics::update(double time_hrs)
{
    if (time_hrs < 0.0)
        throw std::invalid_argument(
            "ICDKinetics::update: time_hrs must be >= 0 (got " +
            std::to_string(time_hrs) + ")");

    // First-order saturation kinetics: C(t) = V * D * (1 - exp(-k * t))
    //   V = volume of necrotic tissue contributing DAMPs
    //   D = death fraction (dose-dependent)
    //   k = marker-specific release rate constant

    // CRT is exposed on the outer membrane leaflet early in the ICD cascade.
    current_crt   = tumor_volume * death_fraction * (1.0 - std::exp(-k_CRT   * time_hrs));

    // ATP is released rapidly as cytoplasmic membranes rupture.
    current_atp   = tumor_volume * death_fraction * (1.0 - std::exp(-k_ATP   * time_hrs));

    // HMGB1 translocates from the nucleus after chromatin decondensation —
    // a measurably slower process than cytoplasmic release.
    current_hmgb1 = tumor_volume * death_fraction * (1.0 - std::exp(-k_HMGB1 * time_hrs));
}

// -----------------------------------------------------------------------------
// get_dc_maturation_probability()
// -----------------------------------------------------------------------------
double ICDKinetics::get_dc_maturation_probability() const
{
    // Combined DAMP signal drives DC maturation via PRR engagement.
    // Hill equation (n=1) models cooperative but saturable receptor binding.
    double signal = current_crt + current_atp + current_hmgb1;
    return signal / (signal + K_HALF);
}
