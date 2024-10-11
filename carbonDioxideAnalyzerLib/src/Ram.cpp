#include "Ram.hpp"
#include <fstream>
#include <algorithm>
#include <numeric>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

double Ram::getUsage() const 
{
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    DWORDLONG physMemUsed = totalPhysMem - memInfo.ullAvailPhys;
    return static_cast<double>(physMemUsed) / totalPhysMem * 100.0;
#else
    std::ifstream meminfo("/proc/meminfo");
    std::string file_contents((std::istreambuf_iterator<char>(meminfo)), std::istreambuf_iterator<char>());

    std::istringstream iss(file_contents);
    std::string line;
    
    long long total = 0, free = 0, buffers = 0, cached = 0;

    while (std::getline(iss, line)) {
        std::istringstream line_stream(line);
        std::string key;
        long long value;

        if (line_stream >> key >> value) {
            if (key == "MemTotal:") total = value;
            else if (key == "MemFree:") free = value;
            else if (key == "Buffers:") buffers = value;
            else if (key == "Cached:") cached = value;
        }
    }

    long long used = total - free - buffers - cached;
    return static_cast<double>(used) / total * 100.0;
#endif
}

double Ram::getTemperature() const 
{
    // RAM temperature is typically not directly measurable
    // This is a placeholder implementation
#ifdef _WIN32
    return 0.0;
#else
    // On Linux, we might be able to read from sensors if available
    std::ifstream temp_input("/sys/class/thermal/thermal_zone0/temp");
    if (temp_input.is_open()) {
        double temp;
        temp_input >> temp;
        return temp / 1000.0;  // Convert from millidegree Celsius to Celsius
    }
    return 0.0;
#endif
}

double Ram::getPowerConsumption() const 
{
    // RAM power consumption is typically not directly measurable
    // This is a placeholder implementation
#ifdef _WIN32
    return 0.0;
#else
    // On Linux, we might be able to estimate based on usage
    double usage = getUsage();
    // Assuming a very rough estimate of 0.1W per GB at full usage
    double estimatedFullPower = 0.1 * (getTotalMemory() / 1024.0);
    return (usage / 100.0) * estimatedFullPower;
#endif
}

long long Ram::getTotalMemory() const
{
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return static_cast<long long>(memInfo.ullTotalPhys / 1024 / 1024);  // Convert to MB
#else
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.compare(0, 9, "MemTotal:") == 0) {
            std::istringstream iss(line);
            std::string key;
            long long value;
            if (iss >> key >> value) {
                return value / 1024;  // Convert from KB to MB
            }
        }
    }
    return 0;
#endif
}
