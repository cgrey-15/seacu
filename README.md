# Introduction

**Seacu** (*si* + *queue*, nicknamed after seacucumber). A tiny collection
of miscellaneous tools, written in C++20 (**currently developed on Windows
using MSVC and Clang toolset**).

# Getting started

**Note:** *Cygwin, MSYS, and Unix-like
environments is not tested and supported.*

## Prerequisites

### Tools
* **required:** A recent C++ compiler with C++20 support; Microsoft Visual Studio 2022 is recommended
* **required:** CMake 3.21+ from KitWare
* [Ninja](https://ninja-build.org) build
* [vcpkg](https://github.com/microsoft/vcpkg) from Microsoft

### Dependncies
* Boost 1.79+
  - Unordered
  - Container Hash
* [Microsoft's Guidlines support library](https://github.com/Microsoft/GSL) (Microsoft.GSL)
* [cxxopts](https://github.com/jarro2783/cxxopts) command-line parsing library
* [PEGTL](https://github.com/taocpp/PEGTL) parser library

## Building seacu
```
git clone https://www.github.com/<user>/seacu.git

cd seacu
```
Using the included `example-windows` CMake preset (uses Ninja generator):
```
cmake -DCMAKE_TOOLCHAIN_FILE=[...] --preset=example-windows
...
<CMake-output>
...
ninja -C ./out/build/example-windows
```
To automatically add all dependencies (and reside in the build tree), add `-DVCPKG_MANIFEST_MODE=ON`:
```
cmake -DCMAKE_TOOLCHAIN_FILE=[...] -DVCPKG_MANIFEST_MODE=ON --preset=example-windows
```

## Basics
*TODO*

