cmake_minimum_required(VERSION 3.21)

set(SEACUDIFF_MAIN_SOURCES "main.cpp")

set(SEACUDIFF_SOURCES 
   diff_histogram.h    diff_histogram.cpp
        seacudiff.h         seacudiff.cpp
    diff_standard.h     diff_standard.tpp diff_standard.cpp
    UmberSequence.h     UmberSequence.cpp
SequenceProcessor.h SequenceProcessor.cpp 
        diff_emit.h         diff_emit.cpp
   SeacudiffTypes.h
SimpleSequence.cpp
EditsSequenceTransformer.hpp)

add_library(seacudiff ${SEACUDIFF_SOURCES})
target_include_directories(seacudiff INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if(SEACU_ASAN_ENABLED)
 message(NOTICE "Seacudiff: Building with Address Sanitizer enabled.")
 if(MSVC)
  if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR SEACUDIFF_FORCE_ASAN_ENABLED)
   target_compile_options(seacudiff PUBLIC /fsanitize=address)
  else()
   message(SEND_ERROR "Currently unable to support Address Sanitizer with MSVC-style Clang. Use -DSEACUDIFF_FORCE_ASAN_ENABLED to generate build proccess anyway.")
  endif()
 else()
  target_compile_options(seacudiff PUBLIC -fsanitize=address)
  target_link_options(seacudiff PUBLIC -fsanitize=address)
 endif()
endif()

add_executable(libseacudiff_runner ${SEACUDIFF_MAIN_SOURCES})
target_link_libraries(libseacudiff_runner cxxopts::cxxopts)

target_compile_features(seacudiff PUBLIC cxx_std_20)
target_link_libraries(seacudiff PRIVATE Boost::headers Microsoft.GSL::GSL)
if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
 target_link_libraries(seacudiff PRIVATE fmt::fmt)
endif()
target_link_libraries(libseacudiff_runner seacudiff)

add_library(seacu::seacudiff ALIAS seacudiff)

install(TARGETS seacudiff libseacudiff_runner EXPORT SeacuProjectTargets
 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
 ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
 INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(FILES seacudiff.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_custom_command(TARGET seacudiff POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/seacudiff.natvis $<TARGET_FILE_DIR:$<TARGET_PROPERTY:NAME>>)

