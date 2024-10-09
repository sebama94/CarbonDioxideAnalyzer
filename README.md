# CO2 Analyzer for PC Resource Usage

## Overview
This project aims to create a tool that analyzes the carbon dioxide (CO2) emissions associated with the usage of CPU, RAM, and GPU in personal computers. By monitoring these hardware components, we can estimate the energy consumption and corresponding CO2 emissions, helping users understand their computer's environmental impact.

## Features
- Real-time monitoring of CPU, RAM, and GPU usage
- Estimation of power consumption based on hardware utilization
- Calculation of CO2 emissions using regional carbon intensity data
- User-friendly interface to display resource usage and estimated emissions

## Getting Started
### Prerequisites
- CMake (version 3.12 or higher)
- C++23 compatible compiler

### Building the Project
1. Clone the repository
2. Navigate to the project directory
3. Run the following commands:
   ```
   mkdir build
   cd build
   cmake ..
   make
   ```

### Running the Analyzer
After building, you can run the analyzer using: