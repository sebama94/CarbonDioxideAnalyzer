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
    Cpu() = default;
    ~Cpu() override = default;
    Cpu(const Cpu&) = delete;
    Cpu& operator=(const Cpu&) = delete;
    Cpu(Cpu&&) = delete;
    Cpu& operator=(Cpu&&) = delete;

    double getUsage() const override;
    double getTemperature() const override;
    double getPowerConsumption() const override;

private:
#ifdef _WIN32
    mutable FILETIME idleTime{};
    mutable FILETIME kernelTime{};
    mutable FILETIME userTime{};
#else
    mutable unsigned long long lastTotalUser{0};
    mutable unsigned long long lastTotalUserLow{0};
    mutable unsigned long long lastTotalSys{0};
    mutable unsigned long long lastTotalIdle{0};
#endif
};
