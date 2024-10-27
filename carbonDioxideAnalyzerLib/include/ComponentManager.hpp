#pragma once

#include "HwMachine.hpp"
#include "Cpu.hpp"
#include "Ram.hpp"
#ifdef GPU_SUPPORT
#include "Gpu.hpp"
#endif
#include "GuiMonitoring.hpp"
#include <chrono>
#include <memory>
#include "SynchronizedQueue.hpp"
#include "Units.hpp"
#include <coroutine>

class ComponentManager 
{
public:
    ComponentManager();
    ~ComponentManager();
    enum class MachineType {
        CPU,
        RAM,
        #ifdef GPU_SUPPORT
        GPU
        #endif
    };
    static std::unique_ptr<HwMachine> createMachine(MachineType type);
    void updateGuiDataThread();
    void runAll();

private:
    std::unique_ptr<Cpu> _cpu;
    std::unique_ptr<Ram> _ram;
    #ifdef GPU_SUPPORT
    std::unique_ptr<Gpu> _gpu;
    #endif
    GuiMonitoring _guiMonitoring;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    SynchronizedQueue<AllComponentData> dataQueue;
    void dataCollectionThread();
    CpuData collectCpuData();
    #ifdef GPU_SUPPORT
    GpuData collectGpuData();
    #endif  
    RamData collectRamData();
};