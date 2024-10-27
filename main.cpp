#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "ComponentManager.hpp"

int main() 
{
    auto componentManager = ComponentManager();

    // Run the GUI (Note: This will block until the GUI window is closed)
    componentManager.runAll();

    return 0;
}
