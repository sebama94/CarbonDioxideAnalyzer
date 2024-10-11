cmake_minimum_required(VERSION 3.12)
project(UnitTests)

# Enable C++23 features
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Ninja as the generator
set(CMAKE_GENERATOR "Ninja")

# Find or fetch Google Test
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG release-1.11.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/carbonDioxideAnalyzerLib/include)
include_directories(${CMAKE_SOURCE_DIR}/carbonDioxideAnalyzerLib/utility)

# Source files
file(GLOB TEST_SOURCES "src/*.cpp")

# Create test executable
add_executable(run_tests ${TEST_SOURCES} unitTest.cpp)

# Link libraries
target_link_libraries(run_tests 
    PRIVATE 
    GTest::GTest
    GTest::Main
    CarbonDioxideAnalyzerLib
)

# Enable testing
enable_testing()

# Add test
add_test(NAME UnitTests COMMAND run_tests)

# Set output directory
if(WIN32)
    set_target_properties(run_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
else()
    set_target_properties(run_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# Execute tests when building the project
if(WIN32)
    add_custom_command(
        TARGET run_tests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running tests..."
        COMMAND ${CMAKE_BINARY_DIR}/bin/run_tests.exe
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
else()
    add_custom_command(
        TARGET run_tests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running tests..."
        COMMAND ${CMAKE_BINARY_DIR}/bin/run_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
