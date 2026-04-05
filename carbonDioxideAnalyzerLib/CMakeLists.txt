cmake_minimum_required(VERSION 3.12)
project(CarbonDioxideAnalyzerLib VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O3 -march=native -mtune=native)
elseif(MSVC)
    add_compile_options(/O2 /arch:AVX2)
endif()

# Link Time Optimization
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO not supported: ${ipo_error}")
endif()

# GTKmm
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-4.0)

if(NOT GTKMM_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        gtkmm
        GIT_REPOSITORY https://gitlab.gnome.org/GNOME/gtkmm.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(gtkmm)
endif()

file(GLOB_RECURSE SOURCES         "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE UTILITY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/utility/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${UTILITY_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/utility>
    $<INSTALL_INTERFACE:include>
    ${GTKMM_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${GTKMM_LIBRARIES} loggerCpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/utility/
    DESTINATION include/${PROJECT_NAME}/utility
    FILES_MATCHING PATTERN "*.hpp"
)

enable_testing()
