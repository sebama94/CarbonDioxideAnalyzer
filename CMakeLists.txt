cmake_minimum_required(VERSION 3.15)
project(CarbonDioxideAnalyzer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler optimizations + warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -O3 -march=native -mtune=native
        -Wall -Wextra -Wpedantic -Werror
        # loggerCpp macros use ##__VA_ARGS__ (GNU extension) — suppress where
        # that header is included in our own translation units.
        -Wno-gnu-zero-variadic-macro-arguments
    )
elseif(MSVC)
    add_compile_options(/O2 /arch:AVX2 /W4 /WX)
endif()

# Link Time Optimization
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO not supported: ${ipo_error}")
endif()

# ── loggerCpp (git submodule: third_party/loggerCpp) ──────────────────────
add_subdirectory(third_party/loggerCpp)

# loggerCpp has unused helpers in headers — suppress only for that target.
target_compile_options(loggerCpp PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:
        -Wno-unused-function
        -Wno-unused-parameter
    >
    $<$<CXX_COMPILER_ID:MSVC>:/wd4100 /wd4505>
)
# ──────────────────────────────────────────────────────────────────────────

add_subdirectory(carbonDioxideAnalyzerLib)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/carbonDioxideAnalyzerLib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/carbonDioxideAnalyzerLib/utility
)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE CarbonDioxideAnalyzerLib loggerCpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# Platform threading
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE wsock32 ws2_32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()

add_subdirectory(unitTest)
