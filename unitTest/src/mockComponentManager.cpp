#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "ComponentManager.hpp"
#include "SynchronizedQueue.hpp"

// ---------------------------------------------------------------------------
// CPU tests
// ---------------------------------------------------------------------------
class CpuTest : public ::testing::Test {
protected:
    std::unique_ptr<HwMachine> cpu{ComponentManager::createMachine(ComponentManager::MachineType::CPU)};
};

TEST_F(CpuTest, UsageInRange) {
    double usage = cpu->getUsage();
    EXPECT_GE(usage, 0.0);
    EXPECT_LE(usage, 100.0);
}

TEST_F(CpuTest, TemperatureInRange) {
    double temp = cpu->getTemperature();
    EXPECT_GE(temp, 0.0);
    EXPECT_LE(temp, 120.0);  // >100°C is technically possible under throttling
}

TEST_F(CpuTest, PowerConsumptionNonNegative) {
    // May be 0.0 if RAPL is not accessible (e.g. CI env without /sys/class/powercap)
    EXPECT_GE(cpu->getPowerConsumption(), 0.0);
}

TEST_F(CpuTest, CO2EmissionNonNegative) {
    // CO2 = power * 0.001 * 450; must be >= 0
    EXPECT_GE(cpu->computeCO2Emission(), 0.0);
}

TEST_F(CpuTest, CO2FormulaProportionalToPower) {
    // The formula is: power_W * 0.001 * 450 gCO2/h
    double power = cpu->getPowerConsumption();
    double co2   = cpu->computeCO2Emission();
    EXPECT_NEAR(co2, power * 0.001 * 450.0, 1e-9);
}

// ---------------------------------------------------------------------------
// RAM tests
// ---------------------------------------------------------------------------
class RamTest : public ::testing::Test {
protected:
    std::unique_ptr<HwMachine> ram{ComponentManager::createMachine(ComponentManager::MachineType::RAM)};
};

TEST_F(RamTest, UsageInRange) {
    double usage = ram->getUsage();
    EXPECT_GE(usage, 0.0);
    EXPECT_LE(usage, 100.0);
}

TEST_F(RamTest, TemperatureNonNegative) {
    EXPECT_GE(ram->getTemperature(), 0.0);
}

TEST_F(RamTest, PowerConsumptionNonNegative) {
    EXPECT_GE(ram->getPowerConsumption(), 0.0);
}

TEST_F(RamTest, CO2EmissionNonNegative) {
    EXPECT_GE(ram->computeCO2Emission(), 0.0);
}

// ---------------------------------------------------------------------------
// Factory tests
// ---------------------------------------------------------------------------
TEST(FactoryTest, CreatesCpuCorrectType) {
    auto machine = ComponentManager::createMachine(ComponentManager::MachineType::CPU);
    ASSERT_NE(machine, nullptr);
    EXPECT_NE(dynamic_cast<Cpu*>(machine.get()), nullptr);
}

TEST(FactoryTest, CreatesRamCorrectType) {
    auto machine = ComponentManager::createMachine(ComponentManager::MachineType::RAM);
    ASSERT_NE(machine, nullptr);
    EXPECT_NE(dynamic_cast<Ram*>(machine.get()), nullptr);
}

// ---------------------------------------------------------------------------
// SynchronizedQueue thread-safety tests
// ---------------------------------------------------------------------------
TEST(SynchronizedQueueTest, PushPopSingleThread) {
    SynchronizedQueue<int> q;
    q.push(42);
    auto val = q.pop();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 42);
}

TEST(SynchronizedQueueTest, TryPopEmptyReturnsFalse) {
    SynchronizedQueue<int> q;
    int out = -1;
    EXPECT_FALSE(q.try_pop(out));
    EXPECT_EQ(out, -1);
}

TEST(SynchronizedQueueTest, TryPopNonEmptyReturnsValue) {
    SynchronizedQueue<int> q;
    q.push(99);
    int out = 0;
    EXPECT_TRUE(q.try_pop(out));
    EXPECT_EQ(out, 99);
}

TEST(SynchronizedQueueTest, ConcurrentPushPop) {
    SynchronizedQueue<int> q;
    constexpr int N = 1000;
    std::atomic<int> sum{0};

    std::thread producer([&]() {
        for (int i = 0; i < N; ++i) q.push(i);
    });

    std::thread consumer([&]() {
        int received = 0;
        while (received < N) {
            int val;
            if (q.try_pop(val)) {
                sum += val;
                ++received;
            }
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(sum.load(), N * (N - 1) / 2);
}

TEST(SynchronizedQueueTest, SizeAndEmpty) {
    SynchronizedQueue<double> q;
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0u);
    q.push(1.0);
    q.push(2.0);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(q.size(), 2u);
}
