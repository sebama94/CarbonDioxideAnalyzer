cmake_minimum_required(VERSION 3.12)
project(CarbonDioxideAnalyzer VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the library
add_subdirectory(carbonDioxideAnalyzerLib)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/carbonDioxideAnalyzerLib/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/carbonDioxideAnalyzerLib/utility)

# Add the executable
add_executable(${PROJECT_NAME} main.cpp)

# Link the library
target_link_libraries(${PROJECT_NAME} PRIVATE CarbonDioxideAnalyzerLib)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install target
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION bin
)
