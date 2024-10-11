#include "ComponentManager.hpp"

std::unique_ptr<HwMachine> HwMachineFactory::createMachine(MachineType type) {
    switch (type) {
        case MachineType::CPU:
            return std::make_unique<Cpu>();
        case MachineType::RAM:
            return std::make_unique<Ram>();
        // case MachineType::GPU:
        //     return std::make_unique<Gpu>();
        default:
            throw std::invalid_argument("Invalid machine type");
    }
}
