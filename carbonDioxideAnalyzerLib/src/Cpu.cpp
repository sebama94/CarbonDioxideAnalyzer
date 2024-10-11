#include "Cpu.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>
#include <ranges>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")
#else
#include <unistd.h>
#include <sys/sysinfo.h>
#endif

double Cpu::getUsage() const 
{
#ifdef _WIN32
    static PDH_HQUERY cpuQuery;
    static PDH_HCOUNTER cpuTotal;
    static bool initialized = false;

    if (!initialized) {
        PdhOpenQuery(NULL, 0, &cpuQuery);
        PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", 0, &cpuTotal);
        PdhCollectQueryData(cpuQuery);
        initialized = true;
    }

    PDH_FMT_COUNTERVALUE counterVal;

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
    return counterVal.doubleValue;
#else
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);
    
    std::vector<unsigned long long> values;
    unsigned long long value;
    iss.ignore(std::numeric_limits<std::streamsize>::max(), ' '); // Skip the first token (cpu label)
    while (iss >> value) {
        values.push_back(value);
    }

    if (values.size() < 4) {
        return 0.0; // Return 0 if we don't have enough data
    }

    unsigned long long idle = values[3];
    unsigned long long total = std::accumulate(values.begin(), values.end(), 0ULL);

    // Calculate the differences
    unsigned long long totalDiff = total - (lastTotalUser + lastTotalUserLow + lastTotalSys + lastTotalIdle);
    unsigned long long idleDiff = idle - lastTotalIdle;

    // Update the last recorded values
    lastTotalUser = values[0];
    lastTotalUserLow = values[1];
    lastTotalSys = values[2];
    lastTotalIdle = idle;

    // Calculate and return the CPU usage
    return totalDiff > 0 ? (100.0 * (totalDiff - idleDiff) / totalDiff) : 0.0;
#endif
}

double Cpu::getTemperature() const 
{
#ifdef _WIN32
    // Windows implementation
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return 0.0;
    }
    
    DWORD temperature;
    DWORD size = sizeof(DWORD);
    if (RegQueryValueEx(hKey, L"~Temperature", NULL, NULL, reinterpret_cast<LPBYTE>(&temperature), &size) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return 0.0;
    }
    
    RegCloseKey(hKey);
    return static_cast<double>(temperature - 2732) / 10.0; // Convert from deciKelvin to Celsius
#else
    // Linux implementation
    if (std::ifstream file("/sys/class/thermal/thermal_zone0/temp"); file) {
        std::string temp;
        std::getline(file, temp);
        return std::stod(temp) / 1000.0;
    }
    return 0.0;
#endif
}

double Cpu::getPowerConsumption() const 
{
#ifdef _WIN32
    // Windows implementation (placeholder)
    // Note: Getting power consumption on Windows typically requires vendor-specific tools
    return 0.0;
#else
    // Linux implementation
    if (std::ifstream file("/sys/class/power_supply/BAT0/power_now"); file) {
        std::string power;
        std::getline(file, power);
        return std::stod(power) / 1000000.0; // Convert microwatts to watts
    }
    return 0.0;
#endif
}
