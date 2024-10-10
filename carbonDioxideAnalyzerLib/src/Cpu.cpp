#include "Cpu.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <ranges>
#include <algorithm>
#include <numeric>
double Cpu::getUsage() const 
{
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);
    std::vector<int> values;
    std::string token;
    iss >> token; // Skip the first token (cpu label)
    while (iss >> token) {
        if (!token.empty()) {
            values.push_back(std::stoi(token));
        }
    }
    auto [idle, total] = std::accumulate(values.begin(), values.end(), std::pair{0, 0},
        [](std::pair<int, int> acc, int val) {
            return std::pair{acc.first + (val == 3 ? val : 0), acc.second + val};
        });
    return 100.0 * (total - idle) / total;
}

double Cpu::getTemperature() const 
{
    std::ifstream file("/sys/class/thermal/thermal_zone0/temp");
    return std::stod(std::string(std::istreambuf_iterator<char>(file), {})) / 1000.0;
}

double Cpu::getPowerConsumption() const 
{
    std::ifstream file("/sys/class/power_supply/BAT0/power_now");
    return std::stod(std::string(std::istreambuf_iterator<char>(file), {})) / 1000000000.0;
}

