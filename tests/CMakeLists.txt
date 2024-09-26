cmake_minimum_required(VERSION 3.19)

# Set project properties
set(PROJECT_NAME transferease-tests)
set(PROJECT_VERSION_CPP_MIN 17)

# Set project configuration
project(${PROJECT_NAME} LANGUAGES CXX)

# Define project options
## No options available

# Set C++ standard to use
if(DEFINED CMAKE_CXX_STANDARD)
    if(${CMAKE_CXX_STANDARD} LESS ${PROJECT_VERSION_CPP_MIN})
        message(FATAL_ERROR "Project ${PROJECT_NAME} require at least C++ standard ${PROJECT_VERSION_CPP_MIN}")
    endif()
else()
    set(CMAKE_CXX_STANDARD ${PROJECT_VERSION_CPP_MIN})
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "Project \"${PROJECT_NAME}\" compiled with C++ standard ${CMAKE_CXX_STANDARD}")

# Set needed packages
## Example: find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(GTest 1.11.0 REQUIRED)

# Manage tests files
set(PROJECT_HEADERS
    testshelper.h
)

set(PROJECT_SOURCES
    testshelper.cpp

    net/bytesarray_tests.cpp
    net/url_tests.cpp

    tools/enumflag_tests.cpp
)

set(PROJECT_FILES ${PROJECT_HEADERS} ${PROJECT_SOURCES})

# Add files to the test application
add_executable(${PROJECT_NAME} ${PROJECT_FILES})

# Link needed libraries
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE transferease)

# Compile needed definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE TEASE_TESTS_DIR_EXTERNAL_RSC="${CMAKE_CURRENT_SOURCE_DIR}/external-ressources/")

# Specify include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
