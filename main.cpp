#include <iostream>
#include "ComponentManager.hpp"
#include <memory>

// Assuming Cpu is defined in HwMachine.hpp or another included file
// If Cpu is in a namespace, use the correct namespace

int main() 
{

    auto cpu = HwMachineFactory::createMachine(HwMachineFactory::MachineType::CPU);
    auto ram = HwMachineFactory::createMachine(HwMachineFactory::MachineType::RAM);

    //auto cpu_usage = cpu->getUsage();
    //auto cpu_temp = cpu->getTemperature();

    //std::cout << "CPU Temperature: " << cpu_temp << " °C" << std::endl;
    
    return 0;
}


