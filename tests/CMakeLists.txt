# CMakeLists file based on project template repository: https://github.com/legerch/projects-template
## Template file version: 2025-12-26 

cmake_minimum_required(VERSION 3.20)

# Set project properties
set(PROJECT_NAME transferease-tests)

# Set project configuration
## Generic properties
project(${PROJECT_NAME} LANGUAGES CXX)

# Define project options
# Example: option(TEASE_TESTS_ENABLE_FEAT1 "Use to enable/disable feature1 for tests." ON)

# Set needed packages
## Example: find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(GTest REQUIRED)

# Defines useful path variables for easier CMake configuration
set(PROJECT_DIR_TESTS_EXTERNAL_RSC  "${CMAKE_CURRENT_SOURCE_DIR}/external-ressources/")

# Manage library files
## Set tests compilation definitions
set(PROJECT_DEFS_PRIV
    TEASE_TESTS_DIR_EXTERNAL_RSC="${PROJECT_DIR_TESTS_EXTERNAL_RSC}"
)

## Set library linking
### Linking dirs
set(PROJECT_LINKING_DIR

)

### Linking libs
set(PROJECT_LINKING_LIB
    GTest::gtest_main
    transferease
)

## Set tests files
set(FILES_HEADERS
    testshelper.h
)

set(FILES_SOURCES
    testshelper.cpp

    net/bytesarray_tests.cpp
    net/url_tests.cpp

    tools/enumflag_tests.cpp

    version/semver_tests.cpp
)

# Add files to the test application
set(PROJECT_FILES ${FILES_HEADERS} ${FILES_SOURCES})
add_executable(${PROJECT_NAME} ${PROJECT_FILES})

# Link needed libraries
target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_LINKING_DIR})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${PROJECT_LINKING_LIB})

# Specify compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror=return-type -Wshadow -Wlogical-op -Wduplicated-cond -Wduplicated-branches>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Werror=return-type -Wshadow -Wlogical-op -Wduplicated-cond -Wduplicated-branches>
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /wd4251>
)

# Compile needed definitions
## Application tests related
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_DEFS_PRIV})

# Specify include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)