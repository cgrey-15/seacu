cmake_minimum_required(VERSION 3.17)

option(VCPKG_MANIFEST_MODE "Automatically get dependencies in <SEACU-ROOT>/vcpkg.json file. Default is 'off'")

project(SeacuProject LANGUAGES CXX)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(fmt CONFIG)
find_package(Boost REQUIRED)
add_subdirectory(seacudiff)
