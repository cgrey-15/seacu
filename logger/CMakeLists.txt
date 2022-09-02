
cmake_minimum_required(VERSION 3.21)

include(GenerateExportHeader)
set(log_sources seaculogger.cpp)
add_library(seaculog ${log_sources})
target_compile_features(seaculog PUBLIC cxx_std_20)
target_include_directories(seaculog PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:include>)
generate_export_header(seaculog BASE_NAME seaculogger)
target_compile_definitions(seaculog PUBLIC $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>:SEACULOGGER_STATIC_DEFINE>)

add_library(seacu::seaculog ALIAS seaculog)

add_library(seaculog-header-only INTERFACE)
target_compile_definitions(seaculog-header-only INTERFACE SEACULOGGER_HEADER_ONLY SEACULOGGER_STATIC_DEFINE)
target_include_directories(seaculog-header-only INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:include>)
target_compile_features(seaculog-header-only INTERFACE cxx_std_20)

add_library(seacu::seaculog-header-only ALIAS seaculog-header-only)

install(TARGETS seaculog seaculog-header-only EXPORT SeacuProjectTargets
 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
 ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
 INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(FILES seaculogger.h seaculogger.hpp "${CMAKE_CURRENT_BINARY_DIR}/seaculogger_export.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
