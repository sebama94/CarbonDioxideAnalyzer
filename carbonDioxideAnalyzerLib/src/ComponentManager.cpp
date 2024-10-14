#include "ComponentManager.hpp"
#include <stdexcept>

ComponentManager::ComponentManager()
    : _cpu{std::unique_ptr<Cpu>(static_cast<Cpu*>(createMachine(MachineType::CPU).release()))}
    , _ram{std::unique_ptr<Ram>(static_cast<Ram*>(createMachine(MachineType::RAM).release()))}
    , _guiMonitoring{std::make_unique<GuiMonitoring>()}
{
#ifdef GPU_SUPPORT
    _gpu = std::unique_ptr<Gpu>(static_cast<Gpu*>(createMachine(MachineType::GPU).release()));
#endif
}

ComponentManager::~ComponentManager() {
    _cpu.reset();
    _ram.reset();
    _guiMonitoring.reset();
#ifdef GPU_SUPPORT
    _gpu.reset();
#endif
}

std::unique_ptr<HwMachine> ComponentManager::createMachine(MachineType type) 
{
    switch (type) {
        case MachineType::CPU:
            return std::make_unique<Cpu>();
        case MachineType::RAM:
            return std::make_unique<Ram>();
        #ifdef GPU_SUPPORT
        case MachineType::GPU:
            return std::make_unique<Gpu>();
        #endif
        default:
            throw std::invalid_argument("Invalid machine type");
    }
}

void ComponentManager::updateGuiData() {
    std::vector<double> data;
    
    // Collect data from CPU
    data.push_back(_cpu->getUsage());
    data.push_back(_cpu->getTemperature());
    data.push_back(_cpu->getPowerConsumption());
    
    // Collect data from RAM
    data.push_back(_ram->getUsage());
    data.push_back(_ram->getTemperature());
    data.push_back(_ram->getPowerConsumption());
    
#ifdef GPU_SUPPORT
    // Collect data from GPU if supported
    data.push_back(_gpu->getUsage());
    data.push_back(_gpu->getTemperature());
    data.push_back(_gpu->getPowerConsumption());
#endif

    // Update GUI with collected data
    _guiMonitoring->updateData(data);
}

void ComponentManager::runAll() {
    _guiMonitoring->runGui();
}
