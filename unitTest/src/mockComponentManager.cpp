#include <gtest/gtest.h>
#include "ComponentManager.hpp"

class ComponentManagerTest : public ::testing::Test {
protected:
    ComponentManager factory;
};

TEST_F(ComponentManagerTest, CpuUsageRange) {
    auto machine = factory.createMachine(ComponentManager::MachineType::CPU);
    auto cpu = dynamic_cast<Cpu*>(machine.get());
    ASSERT_NE(cpu, nullptr);
    
    double usage = cpu->getUsage();
    EXPECT_GE(usage, 0.0);
    EXPECT_LE(usage, 100.0);
}

TEST_F(ComponentManagerTest, CpuTemperatureRange) {
    auto machine = factory.createMachine(ComponentManager::MachineType::CPU);
    auto cpu = dynamic_cast<Cpu*>(machine.get());
    ASSERT_NE(cpu, nullptr);
    
    double temperature = cpu->getTemperature();
    EXPECT_GE(temperature, 0.0);
    EXPECT_LE(temperature, 100.0);  // Assuming temperature is in Celsius
}

TEST_F(ComponentManagerTest, CpuPowerConsumptionPositive) {
    auto machine = factory.createMachine(ComponentManager::MachineType::CPU);
    auto cpu = dynamic_cast<Cpu*>(machine.get());
    ASSERT_NE(cpu, nullptr);
    
    double powerConsumption = cpu->getPowerConsumption();
    EXPECT_GT(powerConsumption, 0.0);
}

TEST_F(ComponentManagerTest, CpuCO2EmissionPositive) {
    auto machine = factory.createMachine(ComponentManager::MachineType::CPU);
    ASSERT_NE(machine, nullptr);
    
    double co2Emission = machine->computeCO2Emission();
    EXPECT_GT(co2Emission, 0.0);
}
