#include <iostream>
#include "loggerCpp/configurationManager.hpp"
#include "ComponentManager.hpp"

int main()
{
    // ── Logger setup ────────────────────────────────────────────────────
    ConfigurationManager logConfig;
    logConfig.applyConsoleSink(utils::LogLevel::DEBUG);
    logConfig.applyFileSink(utils::LogLevel::INFO, "co2analyzer.log");
    // ────────────────────────────────────────────────────────────────────

    LOG_INFO("CO2 Analyzer starting up");

    try {
        ComponentManager componentManager;
        componentManager.runAll();  // blocks until the GUI window is closed
    } catch (const std::exception& e) {
        LOG_CRITICAL("Fatal error: {}", e.what());
        return EXIT_FAILURE;
    }

    LOG_INFO("CO2 Analyzer shut down cleanly");
    return EXIT_SUCCESS;
}
