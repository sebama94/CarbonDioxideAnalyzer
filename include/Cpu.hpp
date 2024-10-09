#include "HwMachine.hpp"

class Cpu : public HwMachine
{
public:
    double getUsage() const override;
    double getTemperature() const override;

    // C++23 features
    auto getUsageAsFloat() const -> float override;

    [[nodiscard("Temperature value should be used")]]
    auto getTemperatureAsFloat() const -> float override;

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