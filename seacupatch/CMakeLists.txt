cmake_minimum_required(VERSION 3.21)

set(seacupatch_sources seacupatch.h seacupatch.cpp)
set(libseacupatch_runner_sources main.cpp platform/ConsoleMisc.h platform/ConsoleMiscWin32.cpp)

add_library(seacupatch ${seacupatch_sources})
target_compile_features(seacupatch INTERFACE cxx_std_17)

target_compile_features(seacupatch PRIVATE cxx_std_20)
target_link_libraries(seacupatch PRIVATE taocpp::pegtl)

if(SEACU_ASAN_ENABLED)
 message(NOTICE "Seacupatch: Building with Address Sanitizer enabled.")
 if(MSVC)
  if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR SEACU_FORCE_ASAN_ENABLED)
   target_compile_options(seacupatch PUBLIC /fsanitize=address)
  else()
   message(SEND_ERROR "Currently unable to support Address Sanitizer with MSVC-style Clang. Use -DSEACU_FORCE_ASAN_ENABLED to generate build proccess anyway.")
  endif()
 else()
  target_compile_options(seacupatch PUBLIC -fsanitize=address)
  target_link_options(seacupatch PUBLIC -fsanitize=address)
 endif()
endif()
add_library(seacu::seacupatch ALIAS seacupatch)
add_executable(libseacupatch_runner ${libseacupatch_runner_sources})
add_executable(seacu::libseacupath_runner ALIAS libseacupatch_runner)
target_compile_features(libseacupatch_runner PRIVATE cxx_std_20)

target_link_libraries(libseacupatch_runner PRIVATE seacupatch cxxopts::cxxopts)

install(TARGETS seacupatch libseacupatch_runner EXPORT SeacuProjectTargets
 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
 ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
 INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(FILES seacupatch.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
