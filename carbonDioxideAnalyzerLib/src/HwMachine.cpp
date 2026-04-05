#include "HwMachine.hpp"

double HwMachine::computeCO2Emission() const
{
    // CO2 emission rate in gCO2/hour:
    //   power (W) × 0.001 (kW/W) × carbon_intensity (gCO2/kWh)
    //
    // Carbon intensity: world average ~450 gCO2/kWh (IEA 2023).
    // Override this constant per-region for more accurate results.
    static constexpr double CARBON_INTENSITY_G_PER_KWH = 450.0;
    static constexpr double W_TO_KW = 0.001;

    return getPowerConsumption() * W_TO_KW * CARBON_INTENSITY_G_PER_KWH;
}
