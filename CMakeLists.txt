cmake_minimum_required(VERSION 3.21)

option(VCPKG_MANIFEST_MODE "Automatically get dependencies in <SEACU-ROOT>/vcpkg.json file. Default is 'off'")
option(SEACU_FORCE_ASAN_ENABLED "Force use of SEACU_ASAN_ENABLED")
option(SEACU_ASAN_ENABLED "Build seacu libraries with AddressSanitizer" ${SEACU_FORCE_ASAN_ENABLED})

project(SeacuProject LANGUAGES CXX)

include(GNUInstallDirs)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(fmt CONFIG)
find_package(Boost REQUIRED)
find_package(argparse)
find_package(cxxopts REQUIRED)
find_package(pegtl CONFIG REQUIRED)

add_subdirectory(logger)
add_subdirectory(seacudiff)
add_subdirectory(seacupatch)
export(EXPORT SeacuProjectTargets FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/SeacuTargets.cmake" NAMESPACE seacu::)
if(SEACU_ENABLE_TESTS)
 if(SeacuProject_IS_TOP_LEVEL)
  include(CTest)
  add_subdirectory(test)
 else()
  message(WARNING "SEACU_ENABLE_TESTS evaluates to TRUE but SeacuProject is root CMake project! Testing currently not supported here.")
 endif()
endif()
