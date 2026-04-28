# PIT-Sim: A High-Performance Simulation Engine for NIR-PIT Kinetics
A high-performance C++ simulation designed to model complex interactions between near-infrared (NIR) light, antibody-photosensitizer conjugates (APCs), and the resulting antitumor immune response, based on current photomedicine. This may translate into a robust software tool for researchers to predict treatment outcomes in various tumor models.


## High-Performance Core: Light Transport
A major hurdle for NIR-PIT is reaching deep-tissue tumors. We can implement a Monte Carlo Light Transport module using OpenMP to simulate how 690 nm photons diffuse through a voxelized 3D tumor grid.

## Biological Modeling: ICD & SUPR Effects
This simulation tracks the unique outcomes of NIR-PIT: the transition from physicochemical changes to Immunogenic Cell Death (ICD). 

* Necrosis Trigger: Simulate the "flood" of water into the cell as membrane integrity degrades.
* DAMPs Release: Model the release of calreticulin (CRT), ATP, and HMGB1. 
* SUPR Effect: Implement a module that increases the "leakiness" of the fluence_map in perivascular spaces to model the super-enhanced permeability and retention effect.



## Simulation Results

### Figure 1: Temporal Kinetics of Immunogenic Cell Death (ICD) Markers

![NIR-PIT ICD Plot](data/icd_plot.png)

**Description:** This plot illustrates the simulated release of Damage-Associated Molecular Patterns (DAMPs) following a single dose of near-infrared photoimmunotherapy (NIR-PIT) at a fluence of **40 J/cm²**. 

* **ATP (Red Line):** Represents the rapid induction of necrotic cell death and the immediate release of "find-me" signals.
* **HMGB1 (Blue Dashed Line):** Displays the sequential release of nuclear proteins that act as potent DAMPs to promote DC maturation.
* **Biological Validation:** The high plateau corresponds to a **Dendritic Cell Maturation Probability of ~99%**, validating the "Antigen Spreading" theory described in Monaco et al. (2022).