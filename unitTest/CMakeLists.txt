cmake_minimum_required(VERSION 3.15)
project(UnitTests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google Test
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/carbonDioxideAnalyzerLib/include
    ${CMAKE_SOURCE_DIR}/carbonDioxideAnalyzerLib/utility
)

file(GLOB TEST_SOURCES "src/*.cpp")
add_executable(run_tests ${TEST_SOURCES})

target_link_libraries(run_tests
    PRIVATE
    GTest::GTest
    GTest::Main
    CarbonDioxideAnalyzerLib
)

enable_testing()
add_test(NAME UnitTests COMMAND run_tests)

set_target_properties(run_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(
    TARGET run_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Running tests..."
    COMMAND $<TARGET_FILE:run_tests>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
