#pragma once

class HwMachine
{
public:
    virtual ~HwMachine() = default;
    HwMachine() = default;
    HwMachine(const HwMachine&) = delete;
    HwMachine& operator=(const HwMachine&) = delete;
    HwMachine(HwMachine&&) = delete;
    HwMachine& operator=(HwMachine&&) = delete;

    virtual double getUsage() const = 0;
    virtual double getTemperature() const { return 0.0; }
    virtual double getPowerConsumption() const = 0;

    // Returns instantaneous CO2 emission rate in gCO2/hour.
    double computeCO2Emission() const;
};
