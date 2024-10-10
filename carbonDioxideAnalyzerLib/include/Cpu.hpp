#pragma once

#include "HwMachine.hpp"

class Cpu : public HwMachine
{
public:
    double getUsage() const override;
    double getTemperature() const override;
    double getPowerConsumption() const override;

    // Destructor
    ~Cpu() override = default;

    // Default constructor
    Cpu() = default;

    // Delete copy and move operations
    Cpu(const Cpu&) = delete;
    Cpu& operator=(const Cpu&) = delete;
    Cpu(Cpu&&) = delete;
    Cpu& operator=(Cpu&&) = delete;
};