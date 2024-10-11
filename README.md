# CO2 Analyzer for PC Resource Usage

## Project Structure
### Core Components

#### HwMachine (Abstract Base Class)
- Defines the interface for hardware components (CPU, RAM, GPU)
- Pure virtual methods: `getUsage()`, `getTemperature()`, `getPowerConsumption()`

#### Concrete Classes
- `Cpu`: Implements HwMachine for CPU-specific functionality
- `Ram`: Implements HwMachine for RAM-specific functionality
- `Gpu`: Implements HwMachine for GPU-specific functionality

#### HwMachineFactory
- Implements the Factory Method pattern
- Creates instances of concrete HwMachine subclasses based on the requested type

### Design Patterns

#### Factory Method Pattern
- Implemented in `HwMachineFactory`
- Allows for flexible creation of different hardware component objects
- Encapsulates object creation logic, making it easier to extend with new hardware types

### Unit Testing

#### Google Test Framework
- Used for writing and running unit tests
- Includes mock objects for testing hardware components in isolation

#### Mock Objects
- `MockCpu`: Mocks the Cpu class for unit testing
- Allows for testing CPU-related functionality without actual hardware dependencies

#### Test Cases
- Usage range tests
- Temperature range tests
- Power consumption tests

### Build System

#### CMake
- Used for cross-platform build configuration
- Manages dependencies, compilation flags, and build targets

### Getting Started
1. Clone the repository
2. Navigate to the project directory
3. Build the project:
   ```
   mkdir build && cd build
   cmake ..
   make
   ```
4. Run the main program:
   ```
   ./bin/CarbonDioxideAnalyzer
   ```
5. Run unit tests:
   ```
   ./bin/run_tests
   ```

This project structure allows for easy extension and maintenance of the CO2 Analyzer, with clear separation of concerns and robust testing capabilities.
