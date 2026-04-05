#include <iostream>
#include "ComponentManager.hpp"  // pulls in gtkmm -> syslog.h (LOG_INFO=6 etc.)
#include "loggerCpp/configurationManager.hpp"
#include "loggerCpp/logMacros.hpp"  // must be last: re-undefs syslog integers, restores our macros

int main()
{
    // ── Logger setup ────────────────────────────────────────────────────
    ConfigurationManager logConfig;
    logConfig.applyConsoleSink(utils::LogLevel::DEBUG);
    logConfig.applyFileSink("co2analyzer.log", utils::LogLevel::INFO);
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
