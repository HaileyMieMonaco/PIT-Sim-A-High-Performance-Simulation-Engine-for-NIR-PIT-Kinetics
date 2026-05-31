"""
plot_results.py
---------------
Visualises the DAMP release kinetics and DC maturation probability produced
by the PIT-Sim C++ engine.

Run from the repository root:
    python scripts/plot_results.py

Or from any directory — the script resolves the CSV path relative to itself.
"""

from pathlib import Path
import sys

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

# ---------------------------------------------------------------------------
# Resolve paths regardless of where the script is called from
# ---------------------------------------------------------------------------
REPO_ROOT = Path(__file__).resolve().parent.parent
CSV_PATH  = REPO_ROOT / "data" / "simulation_results.csv"
OUT_PATH  = REPO_ROOT / "data" / "icd_plot.png"

if not CSV_PATH.exists():
    print(f"Error: {CSV_PATH} not found.\n"
          "Build and run pit_sim first to generate simulation data.", file=sys.stderr)
    sys.exit(1)

data = pd.read_csv(CSV_PATH)

# ---------------------------------------------------------------------------
# Plot layout: DAMP concentrations (top) + DC maturation probability (bottom)
# ---------------------------------------------------------------------------
fig = plt.figure(figsize=(11, 7))
gs  = gridspec.GridSpec(2, 1, height_ratios=[2, 1], hspace=0.35)

# --- Top panel: DAMP release -----------------------------------------------
ax1 = fig.add_subplot(gs[0])

ax1.plot(data["Hour"], data["CRT"],   label="Calreticulin (CRT)",
         color="#e05c5c", linewidth=2, linestyle="-")
ax1.plot(data["Hour"], data["ATP"],   label="ATP (cytoplasmic DAMP)",
         color="#c0392b", linewidth=2, linestyle="--")
ax1.plot(data["Hour"], data["HMGB1"], label="HMGB1 (nuclear DAMP)",
         color="#2980b9", linewidth=2, linestyle="-.")

ax1.set_title(
    "NIR-PIT Induced DAMP Release Kinetics\n"
    "(Monte Carlo fluence → ICD biology pipeline)",
    fontsize=13, fontweight="bold"
)
ax1.set_xlabel("Time Post-Irradiation (hours)")
ax1.set_ylabel("Concentration (relative units)")
ax1.legend(loc="lower right", framealpha=0.85)
ax1.grid(True, alpha=0.3)
ax1.set_xlim(left=0)

# --- Bottom panel: DC maturation probability --------------------------------
ax2 = fig.add_subplot(gs[1])

ax2.fill_between(data["Hour"], data["DC_Maturation_Prob"],
                 alpha=0.25, color="#27ae60")
ax2.plot(data["Hour"], data["DC_Maturation_Prob"],
         color="#27ae60", linewidth=2, label="DC Maturation Probability")
ax2.axhline(0.90, color="gray", linestyle=":", linewidth=1.2,
            label="90% maturation threshold")

ax2.set_xlabel("Time Post-Irradiation (hours)")
ax2.set_ylabel("Probability")
ax2.set_ylim(0, 1.05)
ax2.legend(loc="lower right", framealpha=0.85)
ax2.grid(True, alpha=0.3)
ax2.set_xlim(left=0)

# ---------------------------------------------------------------------------
fig.text(0.01, 0.01,
         "Monaco et al. (2022) — PIT-Sim simulation engine",
         fontsize=8, color="gray")

plt.savefig(OUT_PATH, dpi=150, bbox_inches="tight")
print(f"Figure saved to {OUT_PATH}")
