#pragma once

#include "HwMachine.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

class Ram : public HwMachine
{
public:
    double getUsage() const override;
    double getTemperature() const override;
    double getPowerConsumption() const override;
    long long getTotalMemory() const;

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
