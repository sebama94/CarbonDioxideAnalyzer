#pragma once
#include <iostream>


class HwMachine 
{
    public:
        virtual double getUsage() const = 0;
        virtual double getTemperature() const { return 0.0; }   
        virtual ~HwMachine() = default;
        virtual double getPowerConsumption() const = 0;
        
        HwMachine() = default;
        HwMachine(const HwMachine&) = delete;
        HwMachine& operator=(const HwMachine&) = delete;
        HwMachine(HwMachine&&) = delete;
        HwMachine& operator=(HwMachine&&) = delete;

        double computeCO2Emission() const;

};