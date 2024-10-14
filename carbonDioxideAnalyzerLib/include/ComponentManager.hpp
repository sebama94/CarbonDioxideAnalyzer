#pragma once

#include <memory>
#include "HwMachine.hpp"
// Forward declarations
#include "Cpu.hpp"
#include "Ram.hpp"
#ifdef GPU_SUPPORT
#include "Gpu.hpp"
#endif
#include "GuiMonitoring.hpp"


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
    void updateGuiData();
    void runAll();

private:
    std::unique_ptr<HwMachine> _cpu;
    std::unique_ptr<HwMachine> _ram;
    #ifdef GPU_SUPPORT
    std::unique_ptr<HwMachine> _gpu;
    #endif
    std::unique_ptr<GuiMonitoring> _guiMonitoring;
};
