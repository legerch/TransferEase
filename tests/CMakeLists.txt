cmake_minimum_required(VERSION 3.19)

# Set project properties
set(PROJECT_NAME transferease-tests)
set(PROJECT_VERSION_CPP_MIN 17)

# Set project configuration
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
find_package(GTest 1.11.0 REQUIRED)

# Manage tests files
set(PROJECT_HEADERS
    testshelper.h
)

set(PROJECT_SOURCES
    testshelper.cpp

    net/bytesarray_tests.cpp
    net/url_tests.cpp
)

set(PROJECT_UI

)

set(PROJECT_RSC

)

set(PROJECT_FILES ${PROJECT_HEADERS} ${PROJECT_SOURCES} ${PROJECT_UI} ${PROJECT_RSC})

# Platform dependant stuff
# Windows (for both x86/x64)
if(WIN32)   
    SET(PROJECT_BUILD_ARGS "")
endif()

# MacOS (for both x86/x64)
if(UNIX AND APPLE)   
    SET(PROJECT_BUILD_ARGS "")
endif()

# Linux, BSD, Solaris, Minix (for both x86/x64)
if(UNIX AND NOT APPLE)   
    SET(PROJECT_BUILD_ARGS "")
endif()

# Add files to the test application
add_executable(${PROJECT_NAME} ${PROJECT_BUILD_ARGS} ${PROJECT_FILES})

# Link needed libraries
## GoogleTest library
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main)

# Custom library
target_link_libraries(${PROJECT_NAME} PRIVATE transferease)

# Compile needed definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE TEASE_TESTS_DIR_EXTERNAL_RSC="${CMAKE_CURRENT_SOURCE_DIR}/external-ressources/")
