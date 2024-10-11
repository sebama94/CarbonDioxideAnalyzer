cmake_minimum_required(VERSION 3.12)
project(CarbonDioxideAnalyzer VERSION 1.0 LANGUAGES CXX)

# Enable C++23 features
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Ninja as the generator
set(CMAKE_GENERATOR "Ninja")

# Enable compiler optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O3 -march=native -mtune=native)
elseif(MSVC)
    add_compile_options(/O2 /arch:AVX2)
endif()

# Enable Link Time Optimization (LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)
if(supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO is not supported: ${error}")
endif()

# Enable more warnings and treat them as errors
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
elseif(MSVC)
    add_compile_options(/W4 /WX)
endif()

# Add the libraries
add_subdirectory(carbonDioxideAnalyzerLib)
#add_subdirectory(carbonDioxideGuiMonitoringLib)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/carbonDioxideAnalyzerLib/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/carbonDioxideAnalyzerLib/utility)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/carbonDioxideGuiMonitoringLib/include)

# Add the executable
add_executable(${PROJECT_NAME} main.cpp)

# Link the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE CarbonDioxideAnalyzerLib)
#target_link_libraries(${PROJECT_NAME} PRIVATE CarbonDioxideGuiMonitoringLib)

# Set output directory
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# Install target
if(WIN32)
    install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
    )
else()
    install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
    )
endif()

# Add unitTest folder
add_subdirectory(unitTest)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE wsock32 ws2_32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()
