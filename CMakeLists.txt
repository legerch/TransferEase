cmake_minimum_required(VERSION 3.19)

# Set project properties
set(PROJECT_NAME transferease)
set(PROJECT_VERSION_SEMANTIC 0.0.1)
set(PROJECT_VERSION_CPP_MIN 17)

# Set project configuration
project(${PROJECT_NAME} LANGUAGES CXX)
project(${PROJECT_NAME} VERSION "${PROJECT_VERSION_SEMANTIC}")

# Define project options
option(TEASE_BUILD_TESTS "Use to enable/disable build of unit-tests." ON)

# Where to export generated binaries
if(NOT PROJECT_BUILD_OUTPUT)
    set(PROJECT_BUILD_OUTPUT ${CMAKE_SOURCE_DIR}/build/output/${PROJECT_ARCH_TARGET}/${CMAKE_BUILD_TYPE})

    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BUILD_OUTPUT}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BUILD_OUTPUT}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BUILD_OUTPUT}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_OUTPUT}/bin)
endif()

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
find_package(CURL REQUIRED)

# Defines useful path variables for easier CMake configuration
set(PROJECT_DIR_PUBLIC_HEADERS_ROOT "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_DIR_PUBLIC_HEADERS      "${PROJECT_DIR_PUBLIC_HEADERS_ROOT}/${PROJECT_NAME}")
set(PROJECT_DIR_PRIV_SOURCES        "${PROJECT_SOURCE_DIR}/src")
set(PROJECT_DIR_DOCS_ROOT           "${PROJECT_SOURCE_DIR}/docs")
set(PROJECT_DIR_DOCS_FRAGS          "${PROJECT_DIR_DOCS_ROOT}/fragments")
set(PROJECT_FILE_GLOBAL             "${PROJECT_DIR_PUBLIC_HEADERS}/transferease_global.h")

# Manage library files
## Configure file project - File containing macros that can be used in project
configure_file("${PROJECT_FILE_GLOBAL}.in" "${PROJECT_FILE_GLOBAL}")

## Set library files
### Public interface
set(PROJECT_HEADERS_PUBLIC
    logs/abstractlogger.h

    net/bytesarray.h
    net/request.h
    net/url.h

    tools/enumflag.h

    version/semver.h

    transfermanager.h
)
list(TRANSFORM PROJECT_HEADERS_PUBLIC PREPEND "${PROJECT_DIR_PUBLIC_HEADERS}/")

### Private
set(PROJECT_HEADERS_PRIVATE
    net/handle.h

    tools/filesystemhelper.h
    tools/stringhelper.h
)
list(TRANSFORM PROJECT_HEADERS_PRIVATE PREPEND "${PROJECT_DIR_PRIV_SOURCES}/")

set(PROJECT_SOURCES
    logs/abstractlogger.cpp

    net/bytesarray.cpp
    net/handle.cpp
    net/request.cpp
    net/url.cpp

    tools/filesystemhelper.cpp
    tools/stringhelper.cpp

    version/semver.cpp

    transfermanager.cpp
)
list(TRANSFORM PROJECT_SOURCES PREPEND "${PROJECT_DIR_PRIV_SOURCES}/")

set(PROJECT_FILES ${PROJECT_FILE_GLOBAL} ${PROJECT_HEADERS_PUBLIC} ${PROJECT_HEADERS_PRIVATE} ${PROJECT_SOURCES})

# Add files to the library
add_library(${PROJECT_NAME} SHARED ${PROJECT_FILES})

# Set version of library
set_target_properties(${PROJECT_NAME} PROPERTIES
                        VERSION ${PROJECT_VERSION}
                        SOVERSION ${PROJECT_VERSION_MAJOR})

# Link needed libraries
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

# Specify compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror=return-type -Wshadow -Wlogical-op -Wduplicated-cond -Wduplicated-branches>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Werror=return-type -Wshadow -Wlogical-op -Wduplicated-cond -Wduplicated-branches>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4251>
)

# Compile needed definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE TEASE_LIBRARY_BUILD)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)
endif()

# Specify include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_DIR_PUBLIC_HEADERS_ROOT}  # Only expose public headers
    PRIVATE
        ${PROJECT_DIR_PRIV_SOURCES}         # Internal headers, only for internal use
)

# Do we need to build tests ?
if(TEASE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
