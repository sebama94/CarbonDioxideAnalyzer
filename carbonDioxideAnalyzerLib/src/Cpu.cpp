#include "Cpu.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>
#include <chrono>

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
    iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    while (iss >> value) {
        values.push_back(value);
    }

    if (values.size() < 4) return 0.0;

    unsigned long long idle  = values[3];
    unsigned long long total = std::accumulate(values.begin(), values.end(), 0ULL);

    unsigned long long totalDiff = total - (lastTotalUser + lastTotalUserLow + lastTotalSys + lastTotalIdle);
    unsigned long long idleDiff  = idle  - lastTotalIdle;

    lastTotalUser    = values[0];
    lastTotalUserLow = values[1];
    lastTotalSys     = values[2];
    lastTotalIdle    = idle;

    return totalDiff > 0 ? (100.0 * (totalDiff - idleDiff) / totalDiff) : 0.0;
#endif
}

double Cpu::getTemperature() const
{
#ifdef _WIN32
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                     0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return 0.0;
    }
    DWORD temperature = 0;
    DWORD size = sizeof(DWORD);
    RegQueryValueEx(hKey, L"~Temperature", NULL, NULL,
                    reinterpret_cast<LPBYTE>(&temperature), &size);
    RegCloseKey(hKey);
    return static_cast<double>(temperature - 2732) / 10.0;
#else
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
    return 0.0;
#else
    // 1. Try Intel RAPL (works on desktops and laptops with Intel/AMD CPUs).
    //    Reads energy counter and computes average power since last call.
    {
        static unsigned long long lastEnergy = 0;
        static auto lastTime = std::chrono::steady_clock::now();

        std::ifstream rapl("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj");
        if (rapl) {
            unsigned long long energy = 0;
            rapl >> energy;

            auto now     = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(now - lastTime).count();
            double power   = 0.0;

            if (lastEnergy > 0 && elapsed > 0.0) {
                // energy_uj wraps at max_energy_range_uj; ignore wrap-around samples
                if (energy >= lastEnergy) {
                    power = static_cast<double>(energy - lastEnergy) * 1e-6 / elapsed;
                }
            }
            lastEnergy = energy;
            lastTime   = now;
            return power;
        }
    }

    // 2. Fallback: battery power_now (laptops on battery).
    if (std::ifstream bat("/sys/class/power_supply/BAT0/power_now"); bat) {
        unsigned long long uW = 0;
        bat >> uW;
        return static_cast<double>(uW) * 1e-6;
    }

    return 0.0;
#endif
}
