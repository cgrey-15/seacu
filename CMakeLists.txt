cmake_minimum_required(VERSION 3.17)

option(VCPKG_MANIFEST_MODE "Automatically get dependencies in <SEACU-ROOT>/vcpkg.json file. Default is 'off'")
option(SEACU_FORCE_ASAN_ENABLED "Force use of SEACU_ASAN_ENABLED")
option(SEACU_ASAN_ENABLED "Build seacu libraries with AddressSanitizer" ${SEACU_FORCE_ASAN_ENABLED})

project(SeacuProject LANGUAGES CXX)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(fmt CONFIG)
find_package(Boost REQUIRED)
find_package(argparse REQUIRED)
find_package(pegtl CONFIG REQUIRED)
add_subdirectory(seacudiff)
add_subdirectory(seacupatch)
if(SEACU_ENABLE_TESTS)
 include(CTest)
 add_subdirectory(test)
endif()
