#pragma once

#include "HwMachine.hpp"

class Ram : public HwMachine
{
public:
    double getUsage() const override;
    double getTemperature() const override;
    double getPowerConsumption() const override;

    // Destructor
    ~Ram() override = default;

    // Default constructor
    Ram() = default;

    // Delete copy and move operations
    Ram(const Ram&) = delete;
    Ram& operator=(const Ram&) = delete;
    Ram(Ram&&) = delete;
    Ram& operator=(Ram&&) = delete;
};
