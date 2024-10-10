#include "Ram.hpp"
#include <fstream>
#include <string>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <sstream>

double Ram::getUsage() const 
{
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
}

double Ram::getTemperature() const 
{
    // RAM temperature is typically not directly measurable
    // This is a placeholder implementation
    return 0.0;
}

double Ram::getPowerConsumption() const 
{
    // RAM power consumption is typically not directly measurable
    // This is a placeholder implementation
    return 0.0;
}
