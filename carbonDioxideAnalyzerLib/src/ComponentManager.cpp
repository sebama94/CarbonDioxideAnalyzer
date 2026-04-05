#include <stdexcept>
#include <memory>
#include <thread>

#include "loggerCpp/configurationManager.hpp"
#include "ComponentManager.hpp"

ComponentManager::ComponentManager()
    : _cpu{std::unique_ptr<Cpu>(static_cast<Cpu*>(createMachine(MachineType::CPU).release()))}
    , _ram{std::unique_ptr<Ram>(static_cast<Ram*>(createMachine(MachineType::RAM).release()))}
{
#ifdef GPU_SUPPORT
    _gpu = std::unique_ptr<Gpu>(static_cast<Gpu*>(createMachine(MachineType::GPU).release()));
#endif
    LOG_DEBUG("ComponentManager initialised");
}

ComponentManager::~ComponentManager() = default;

std::unique_ptr<HwMachine> ComponentManager::createMachine(MachineType type)
{
    switch (type) {
        case MachineType::CPU: return std::make_unique<Cpu>();
        case MachineType::RAM: return std::make_unique<Ram>();
#ifdef GPU_SUPPORT
        case MachineType::GPU: return std::make_unique<Gpu>();
#endif
        default: throw std::invalid_argument("Invalid machine type");
    }
}

void ComponentManager::runAll() {
    LOG_INFO("Starting data collection thread");
    std::jthread dataThread(&ComponentManager::dataCollectionThread, this);
    _guiMonitoring.runGui();   // blocks until window is closed
    LOG_INFO("GUI closed — stopping data collection");
}  // jthread destructor requests stop and joins automatically

void ComponentManager::dataCollectionThread(std::stop_token stop) {
    LOG_INFO("Data collection thread running");
    while (!stop.stop_requested()) {
        AllComponentData allData;
        allData.cpuData = collectCpuData();
        allData.ramData = collectRamData();
#ifdef GPU_SUPPORT
        allData.gpuData = collectGpuData();
#endif
        LOG_DEBUG("CPU {:.1f}% {:.1f}°C {:.2f}W | RAM {:.1f}% {:.2f}W",
            allData.cpuData.usage,
            allData.cpuData.temperature,
            allData.cpuData.powerConsumption,
            allData.ramData.usage,
            allData.ramData.powerConsumption);

        _guiMonitoring.updateData(allData);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LOG_INFO("Data collection thread stopped");
}

CpuData ComponentManager::collectCpuData() {
    return {_cpu->getUsage(), _cpu->getTemperature(), _cpu->getPowerConsumption(),
            std::chrono::steady_clock::now()};
}

RamData ComponentManager::collectRamData() {
    return {_ram->getUsage(), _ram->getTemperature(), _ram->getPowerConsumption(),
            std::chrono::steady_clock::now()};
}

#ifdef GPU_SUPPORT
GpuData ComponentManager::collectGpuData() {
    return {_gpu->getUsage(), _gpu->getTemperature(), _gpu->getPowerConsumption(),
            std::chrono::steady_clock::now()};
}
#endif
