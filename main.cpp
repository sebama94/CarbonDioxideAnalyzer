#include <iostream>
#include "ComponentManager.hpp"  // pulls in gtkmm -> syslog.h (LOG_INFO=6 etc.)

// loggerCpp MUST be included after all headers that transitively include
// <syslog.h>, so that utils.hpp can #undef the integer LOG_* names and
// replace them with our function-like macros.
#include "loggerCpp/configurationManager.hpp"

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
