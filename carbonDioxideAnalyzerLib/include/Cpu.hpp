#pragma once

#include "HwMachine.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

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

private:
#ifdef _WIN32
    mutable FILETIME idleTime;
    mutable FILETIME kernelTime;
    mutable FILETIME userTime;
#else
    mutable unsigned long long lastTotalUser;
    mutable unsigned long long lastTotalUserLow;
    mutable unsigned long long lastTotalSys;
    mutable unsigned long long lastTotalIdle;
#endif
};