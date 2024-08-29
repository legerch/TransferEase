cmake_minimum_required(VERSION 3.19)
project(TransferEaseMain LANGUAGES CXX)

# Defines options of project
# Ex : set(EXT_OPT_LIBRARY_TEASE 0)

# Export generated binaries
if(NOT PROJECT_BUILD_OUTPUT)
    set(PROJECT_BUILD_OUTPUT ${CMAKE_SOURCE_DIR}/build/output/${PROJECT_ARCH_TARGET}/${CMAKE_BUILD_TYPE})

    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_OUTPUT}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BUILD_OUTPUT}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BUILD_OUTPUT}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_OUTPUT}/bin)
endif()

# Build library
add_subdirectory(transferease)

# Do we need to build tests ?
if(NOT PROJECT_TEASE_DISABLE_TESTS)
    add_subdirectory(tests)
endif()
