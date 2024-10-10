#include "HwMachine.hpp"

double HwMachine::computeCO2Emission() const
{
    // This is a simplified calculation and should be adjusted based on actual CO2 emission factors
    // and more complex relationships between hardware usage, temperature, and CO2 emissions.
    const double usageFactor = 0.5;  // CO2 emission factor for usage (example value)
    const double temperatureFactor = 0.3;  // CO2 emission factor for temperature (example value)

    double usage = getUsage();
    double temperature = getTemperature();

    // Calculate CO2 emission based on usage and temperature
    double emission = (usage * usageFactor) + (temperature * temperatureFactor);

    return emission;
}
