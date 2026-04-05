#include "Ram.hpp"
#include <fstream>
#include <sstream>
#include <string>

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
    DWORDLONG total    = memInfo.ullTotalPhys;
    DWORDLONG used     = total - memInfo.ullAvailPhys;
    return static_cast<double>(used) / static_cast<double>(total) * 100.0;
#else
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    long long total = 0, available = 0;

    while (std::getline(meminfo, line)) {
        std::istringstream iss(line);
        std::string key;
        long long value;
        if (iss >> key >> value) {
            if      (key == "MemTotal:")     total     = value;
            else if (key == "MemAvailable:") available = value;
        }
        if (total > 0 && available > 0) break;  // both found — stop early
    }

    if (total == 0) return 0.0;
    return static_cast<double>(total - available) / static_cast<double>(total) * 100.0;
#endif
}

double Ram::getTemperature() const
{
#ifdef _WIN32
    return 0.0;
#else
    if (std::ifstream f("/sys/class/thermal/thermal_zone0/temp"); f) {
        double temp;
        f >> temp;
        return temp / 1000.0;
    }
    return 0.0;
#endif
}

double Ram::getPowerConsumption() const
{
#ifdef _WIN32
    return 0.0;
#else
    // Rough estimate: 0.1 W per GB at full utilisation, scaled by actual usage.
    double usage = getUsage();
    double totalGB = static_cast<double>(getTotalMemory()) / 1024.0;
    return (usage / 100.0) * 0.1 * totalGB;
#endif
}

long long Ram::getTotalMemory() const
{
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return static_cast<long long>(memInfo.ullTotalPhys / 1024 / 1024);  // MB
#else
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.compare(0, 9, "MemTotal:") == 0) {
            std::istringstream iss(line);
            std::string key;
            long long value;
            if (iss >> key >> value) return value / 1024;  // KB → MB
        }
    }
    return 0;
#endif
}
