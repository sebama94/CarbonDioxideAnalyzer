#pragma once

#include <memory>

#include "HwMachine.hpp"
#include "Cpu.hpp"
#include "Ram.hpp"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#ifdef GPU_SUPPORT
    #include "Gpu.hpp"
#endif

class HwMachineFactory {
public:
    enum class MachineType {
        CPU,
        RAM,
        #ifdef GPU_SUPPORT
        GPU
        #endif
    };

    static std::unique_ptr<HwMachine> createMachine(MachineType type);
};
