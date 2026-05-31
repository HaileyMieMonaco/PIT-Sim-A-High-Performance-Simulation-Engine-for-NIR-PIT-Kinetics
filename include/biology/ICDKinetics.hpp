#ifndef ICD_KINETICS_HPP
#define ICD_KINETICS_HPP

/**
 * @class ICDKinetics
 * @brief Models the release of Damage-Associated Molecular Patterns (DAMPs)
 *        and subsequent immune activation following NIR-PIT irradiation.
 *
 * Based on Monaco et al. (2022), "Quickly evolving near-infrared
 * photoimmunotherapy provides multifaceted approach to modern cancer treatment."
 *
 * The three canonical ICD markers modeled here are:
 *   - Calreticulin (CRT) — early "eat-me" signal on the cell surface
 *   - ATP            — rapid "find-me" signal released on membrane rupture
 *   - HMGB1          — late nuclear DAMP that drives DC maturation
 */
class ICDKinetics {
public:
    /// @param initial_tumor_vol  Tumor volume in mm³ (must be > 0)
    /// @param effective_fluence  Mean fluence delivered to the tumor (J/cm²)
    /// @throws std::invalid_argument if parameters are out of range
    explicit ICDKinetics(double initial_tumor_vol, double effective_fluence = 40.0);

    /**
     * @brief Advance the DAMP release model to time_hrs post-irradiation.
     *
     * Cell death fraction follows a Beer–Lambert-inspired dose–response:
     *   death_fraction = 1 - exp(-fluence * 0.1)
     *
     * Individual marker concentrations follow first-order release kinetics
     * (sigmoidal saturation), consistent with rapid necrotic dynamics
     * observed in preclinical NIR-PIT studies (Monaco et al. 2022, §3.2).
     *
     * @param time_hrs  Time since irradiation in hours (must be >= 0)
     */
    void update(double time_hrs);

    // --- Getters --------------------------------------------------------
    double get_crt()   const { return current_crt;   }
    double get_atp()   const { return current_atp;   }
    double get_hmgb1() const { return current_hmgb1; }

    /**
     * @brief Probability that local DAMP concentration is sufficient to
     *        trigger Dendritic Cell (DC) maturation.
     *
     * Uses a Hill equation (n=1) over the combined DAMP signal strength,
     * representing cooperative receptor binding on immature DCs.
     * A DC maturation probability ≥ 0.9 is consistent with the antigen-
     * spreading mechanism described in Monaco et al. (2022), §4.1.
     */
    double get_dc_maturation_probability() const;

private:
    double tumor_volume;    ///< Tumor volume in mm³
    double nir_fluence;     ///< Effective mean fluence (J/cm²)
    double death_fraction;  ///< Fraction of cells killed at this fluence

    // First-order release rate constants (hr⁻¹), derived from preclinical
    // observations of rapid necrosis post-NIR-PIT.
    static constexpr double k_CRT   = 1.20; ///< Early surface exposure
    static constexpr double k_ATP   = 0.85; ///< Rapid cytoplasmic release
    static constexpr double k_HMGB1 = 0.45; ///< Slower nuclear release

    // Hill equation half-saturation constant for DC maturation (relative units)
    static constexpr double K_HALF = 10.0;

    double current_crt   = 0.0;
    double current_atp   = 0.0;
    double current_hmgb1 = 0.0;
};

#endif // ICD_KINETICS_HPP
