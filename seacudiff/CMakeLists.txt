cmake_minimum_required(VERSION 3.17)

set(SEACUDIFF_MAIN_SOURCES "main.cpp")
set(SEACUDIFF_SOURCES "diff_histogram.cpp" "diff_histogram.h" "seacudiff.h" "diff_standard.cpp" "diff_standard.h" "seacudiff.cpp" "UmberSequence.h" "UmberSequence.cpp")

add_library(seacudiff ${SEACUDIFF_SOURCES})
add_executable(test_prog ${SEACUDIFF_MAIN_SOURCES})

target_compile_features(seacudiff PUBLIC cxx_std_20)
target_link_libraries(seacudiff PRIVATE Microsoft.GSL::GSL Boost::headers)
if(True)
 target_link_libraries(seacudiff PRIVATE fmt::fmt)
endif()
target_link_libraries(test_prog seacudiff)

if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "[Mm][Ss][Vv][Cc]")
 target_compile_options(seacudiff PRIVATE /ZI)
endif()

add_custom_command(TARGET seacudiff POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/seacudiff.natvis $<TARGET_FILE_DIR:$<TARGET_PROPERTY:NAME>>)

#target_compile_features(seacudiff PRIVATE cxx_std_17)
