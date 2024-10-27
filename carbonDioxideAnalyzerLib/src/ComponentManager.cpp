#include <stdexcept>
#include <memory>
#include <thread>
#include <iostream>

#include "ComponentManager.hpp"


ComponentManager::ComponentManager()
    : _cpu{std::unique_ptr<Cpu>(static_cast<Cpu*>(createMachine(MachineType::CPU).release()))}
    , _ram{std::unique_ptr<Ram>(static_cast<Ram*>(createMachine(MachineType::RAM).release()))}// Assuming GuiMonitoring is a class
{
#ifdef GPU_SUPPORT
    _gpu = std::unique_ptr<Gpu>(static_cast<Gpu*>(createMachine(MachineType::GPU).release()));
#endif
}

ComponentManager::~ComponentManager() {
    _cpu.reset();
    _ram.reset();
#ifdef GPU_SUPPORT
    _gpu.reset();
#endif
}

std::unique_ptr<HwMachine> ComponentManager::createMachine(MachineType type) 
{
    switch (type) 
    {
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

void ComponentManager::runAll() {
    std::cout << "Debug: ComponentManager::runAll() starting threads" << std::endl;
    //std::jthread guiThread(&GuiMonitoring::runGui, &_guiMonitoring);
    
    std::jthread dataCollectionThread(&ComponentManager::dataCollectionThread, this);
    std::cout << "Debug: ComponentManager::runAll() threads started" << std::endl;
    _guiMonitoring.runGui();
}

void ComponentManager::dataCollectionThread() {
    std::cout << "Debug: ComponentManager::dataCollectionThread() started" << std::endl;
    while (true) {
        AllComponentData allData;
        allData.cpuData = collectCpuData();
        allData.ramData = collectRamData();
        #ifdef GPU_SUPPORT
        std::cout << "Debug: Collecting GPU data" << std::endl;
        allData.gpuData = collectGpuData();
        #endif
        std::cout << "Debug: All data collected:" << std::endl;
        std::cout << "  CPU - Usage: " << allData.cpuData.usage << "%, Temp: " << allData.cpuData.temperature 
                  << "°C, Power: " << allData.cpuData.powerConsumption << "W" << std::endl;
        std::cout << "  RAM - Usage: " << allData.ramData.usage << "%, Temp: " << allData.ramData.temperature 
                  << "°C, Power: " << allData.ramData.powerConsumption << "W" << std::endl;
        #ifdef GPU_SUPPORT
        std::cout << "  GPU - Usage: " << allData.gpuData.usage << "%, Temp: " << allData.gpuData.temperature 
                  << "°C, Power: " << allData.gpuData.powerConsumption << "W" << std::endl;
        #endif
        _guiMonitoring.updateData(allData);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

CpuData ComponentManager::collectCpuData() {
    return {CpuData{_cpu->getUsage(), _cpu->getTemperature(), _cpu->getPowerConsumption(), std::chrono::steady_clock::now()}};
}

RamData ComponentManager::collectRamData() {
    return {RamData{_ram->getUsage(), _ram->getTemperature(), _ram->getPowerConsumption(), std::chrono::steady_clock::now()}};
}

#ifdef GPU_SUPPORT
std::vector<GpuData> ComponentManager::collectGpuData() {
    return {GpuData{_gpu->getUsage(), _gpu->getTemperature(), _gpu->getPowerConsumption(), std::chrono::steady_clock::now()}};
}
#endif

