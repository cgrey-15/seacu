cmake_minimum_required(VERSION 3.17)

option(VCPKG_MANIFEST_MODE "Automatically get dependencies in <SEACU-ROOT>/vcpkg.json file. Default is 'off'")
option(SEACUDIFF_FORCE_ASAN_ENABLED "Force use of SEACUDIFF_ASAN_ENABLED")
option(SEACUDIFF_ASAN_ENABLED "Build libseacudiff with AddressSanitizer" ${SEACUDIFF_FORCE_ASAN_ENABLED})

project(SeacuProject LANGUAGES CXX)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(fmt CONFIG)
find_package(Boost REQUIRED)
find_package(argparse REQUIRED)
add_subdirectory(seacudiff)
if(SEACU_ENABLE_TESTS)
 include(CTest)
 add_subdirectory(test)
endif()
