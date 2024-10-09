#include <iostream>


class HwMachine 
{
    public:
        virtual double getUsage() const = 0;
        virtual double getTemperature() const { return 0.0; }   
        virtual ~HwMachine() = default;
        
        HwMachine() = default;
        HwMachine(const HwMachine&) = delete;
        HwMachine& operator=(const HwMachine&) = delete;
        HwMachine(HwMachine&&) = delete;
        HwMachine& operator=(HwMachine&&) = delete;

        // C++23 features
        constexpr virtual auto getUsageAsFloat() const -> float {
            return static_cast<float>(getUsage());
        }

        [[nodiscard("Temperature value should be used")]]
        constexpr virtual auto getTemperatureAsFloat() const -> float {
            return static_cast<float>(getTemperature());
        }
};