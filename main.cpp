#include <iostream>
#include "ComponentManager.hpp"
#include <memory>
#include <thread>
#include <chrono>

int main() 
{
    auto componentManager = std::make_unique<ComponentManager>();
    
    // Create a thread for updating GUI data periodically
    std::jthread updateThread([&componentManager](std::stop_token stoken) {
        while (!stoken.stop_requested()) {
            componentManager->updateGuiData();
            // Add a small delay to avoid excessive updates
            std::this_thread::sleep_for(std::chrono::milliseconds(6000));
        }
    });

    // Run the GUI (Note: This will block until the GUI window is closed)
    componentManager->runAll();
    
    return 0;
}
