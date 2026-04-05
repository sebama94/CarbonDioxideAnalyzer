#pragma once

#include "HwMachine.hpp"

// Stub GPU implementation.
// To add real support, define GPU_SUPPORT and implement via NVML (NVIDIA)
// or ROCm (AMD). Until then all metrics return 0.0.
class Gpu : public HwMachine
{
public:
    Gpu() = default;
    ~Gpu() override = default;
    Gpu(const Gpu&) = delete;
    Gpu& operator=(const Gpu&) = delete;
    Gpu(Gpu&&) = delete;
    Gpu& operator=(Gpu&&) = delete;

    double getUsage() const override;
    double getTemperature() const override;
    double getPowerConsumption() const override;
};
