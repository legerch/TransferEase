C++ cross-platform library [TransferEase][tease-repo] which will provide methods to download/upload datas easily.

> [!TIP]
> Latest development/pull requests will be committed into `main` branch.  
> Each stable release have their dedicated branch:
> - `1.0.x`: branch `dev/1.0`
> - `1.1.x`: branch `dev/1.1`
> - etc...

**Table of contents :**
- [1. Requirements](#1-requirements)
  - [1.1. C++ Standards](#11-c-standards)
  - [1.2. Dependencies](#12-dependencies)
- [2. Integration](#2-integration)
  - [2.1. How to build](#21-how-to-build)
  - [2.2. CMake options](#22-cmake-options)
- [3. How to use](#3-how-to-use)
  - [3.1. Usage](#31-usage)
  - [3.2. Logs management](#32-logs-management)
  - [3.3. Library version](#33-library-version)
    - [3.3.1. Compilation time](#331-compilation-time)
    - [3.3.2. Runtime](#332-runtime)
- [4. Library details](#4-library-details)
  - [4.1. Implementation](#41-implementation)
  - [4.2. Known issues](#42-known-issues)
    - [4.2.1. Large files](#421-large-files)
- [5. Documentation](#5-documentation)
- [6. License](#6-license)

# 1. Requirements
## 1.1. C++ Standards

This library requires at least **C++ 17** standard

## 1.2. Dependencies

Below, list of required dependencies:

| Dependencies | VCPKG package | Comments |
|:-:|:-:|:-:|
| [libcurl][libcurl-home] | `curl` | / |
| [Google Tests][gtest-repo] | `gtest` | Only needed to run unit-tests |

> Dependency manager [VCPKG][vcpkg-tutorial] is not mandatory, this is only a note to be able to list needed packages

# 2. Integration
## 2.1. How to build
This library can be use as an _embedded library_ in a subdirectory of your project (like a _git submodule_ for example) :
1. In the **root** CMakeLists, add instructions :
```cmake
add_subdirectory(TransferEase) # Or if library is put in a folder "dependencies" : add_subdirectory(dependencies/TransferEase)
```

2. In the **application/library** CMakeLists, add instructions :
```cmake
# Link needed libraries
target_link_libraries(${PROJECT_NAME} PRIVATE transferease)
```

## 2.2. CMake options

This library provide some **CMake** options:
- `TEASE_BUILD_TESTS`: Use to enable/disable unit-tests of the library. **Default value:** `ON`.

# 3. How to use
## 3.1. Usage

Please refer to `TransferManager` class documentation for more details

## 3.2. Logs management

This library use a _virtual logger_ (based on [this implementation][virtual-log-repo]) in order to let users of the library use their own log management mechanism. By default, no logs will be printed. To enable library log usage, caller must provide a class inheriting
from `tease::ILogger` interface.  
More details on how to use this log interface can be found inside [abstract logger repository][virtual-log-repo]

## 3.3. Library version
### 3.3.1. Compilation time

In order to easily check at compilation time library version (to manage compatibility between multiple versions for example), macro `TEASE_VERSION_ENCODE` (defined inside _transferease_global.h_ file) can be used:
```cpp
#if TEASE_VERSION >= TEASE_VERSION_ENCODE(1,0,0)
    // Do stuff for version 1.0.0 or higher
#else
    // Do stuff for version 0.0.x
#endif
```

### 3.3.2. Runtime

Since library header used during final application build could differ from the **actual** library version, it is recommended to use the static method:
```cpp
#include "transferease/version/semver.h"

const Semver &teaseSemver = tease::Semver::getLibraryVersion();
```

# 4. Library details
## 4.1. Implementation

This library use the [PImpl Idiom][pimpl-doc] in order to preserve _ABI compatibility_. So only **major** release (this project use the [semantic versioning][semver-home]) _should_ break the ABI.

## 4.2. Known issues
### 4.2.1. Large files

Currently, library store all downloaded/uploaded datas in _heap memory_, that provide us some advantages:
- For **uploaded** datas, we don't have to load data from a file (we still can) but those can also be runtime generated
- For **downloaded** datas, we can _play_ with received datas directly instead of writing them on the disk and reloading those into memory

But this behaviour also have **downsides** : since all datas are stored into _heap memory_, this could lead to issues when managing large ressources.  
Currently, only **uploaded** datas have a safeguard (when _loading data from file_) set to `1 Gigabytes (1024^3 bytes)`.

> [!WARNING]
> No safeguard currently set for **downloaded** datas !

This issue will be taken care in a future release (via _streams/chunks management_) but I can only recommend to not use this library for now if downloaded files can be large.

# 5. Documentation

All classes/methods has been documented with [Doxygen][doxygen-official] utility and automatically generated at [online website documentation][repo-doc-web].

> [!NOTE]
> This repository contains two kinds of documentation:
> - **Public API:** Available via [online website documentation][repo-doc-web] or locally via Doxyfile `docs/fragments/Doxyfile-public-api.in`
> - **Internal:** Available locally only via `docs/fragments/Doxyfile-internal.in`

To generate documentation locally, we can use:
```shell
# Run documentation generation
doxygen ./Doxyfile-name

# Under Windows OS, maybe doxygen is not added to the $PATH
"C:\Program Files\doxygen\bin\doxygen.exe" ./Doxyfile-name
```
> [!TIP]
> You can also load the _Doxyfile_ into _Doxywizard_ (Doxygen GUI) and run generation.

# 6. License

This library is licensed under [MIT license][repo-license].

<!-- Links of this repository -->
[repo-license]: LICENSE

<!-- External links -->
[doxygen-official]: https://www.doxygen.nl/index.html
[gtest-repo]: https://github.com/google/googletest
[libcurl-home]: https://curl.se/libcurl/
[pimpl-doc]: https://en.cppreference.com/w/cpp/language/pimpl
[semver-home]: https://semver.org
[tease-repo]: https://github.com/legerch/TransferEase
[virtual-log-repo]: https://github.com/legerch/AbstractLogger

[vcpkg-tutorial]: https://github.com/legerch/develop-memo/tree/master/Toolchains/Build%20systems/VCPKG