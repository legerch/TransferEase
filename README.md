- _//TODO: simple project description_
- _//TODO: describe devlopment branch and main branch_

**Table of contents :**
- [1. Requirements](#1-requirements)
  - [1.1. C++ Standards](#11-c-standards)
  - [1.2. Dependencies](#12-dependencies)
- [2. How to build](#2-how-to-build)
- [3. How to use](#3-how-to-use)
- [4. Library details](#4-library-details)
- [5. Documentation](#5-documentation)
- [6. License](#6-license)

# 1. Requirements
## 1.1. C++ Standards

This library requires at least **C++ 17** standard

## 1.2. Dependencies

Below, list of required dependencies:

| Dependencies | VCPKG package | Comments |
|:-:|:-:|:-:|
| [Google Tests][gtest-repo] | `gtest` | Only needed to run unit-tests |

> Dependency manager [VCPKG][vcpkg-tutorial] is not mandatory, this is only a note to be able to list needed packages

# 2. How to build

This library can be use as an _embedded library_ in a subdirectory of your project (like a _git submodule_ for example) :
1. In the **root** CMakeLists, add instructions :
```cmake
add_subdirectory(TransferEase) # Or if library is put in a folder "dependencies" : add_subdirectory(dependencies/TransferEase)
```

1. In the **application/library** CMakeLists, add instructions :
```cmake
# Link needed libraries
target_link_libraries(${PROJECT_NAME} PRIVATE transferease)
```

# 3. How to use

- _//TODO: Describe how to use library_
- _//TODO: virtual logger description_
- _//TODO: how to disable tests at build (use cmake option feature)_
- _//TODO: way to retrieve library version_

# 4. Library details

- _//TODO: Pimpl idiom_
- _//TODO: Talk about large files (about current safeguard of 1Gb + add implementation with stream chunk management)_
- _//TODO: reorganize library for proper include/src folder (pitchfork convention: https://github.com/vector-of-bool/pitchfork/issues/51)_

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

[vcpkg-tutorial]: https://github.com/legerch/develop-memo/tree/master/Toolchains/Build%20systems/VCPKG