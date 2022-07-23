cmake_minimum_required(VERSION 3.19)

set(TEST_SOURCES MergeEdits.cpp)

create_test_sourcelist(Tests MiscTests.cpp ${TEST_SOURCES})

add_executable(MiscTests ${Tests})

target_link_libraries(MiscTests PRIVATE seacudiff)

set(TestsToRun2 ${TEST_SOURCES})

foreach(test ${TestsToRun2})
 get_filename_component(TestName ${test} NAME_WE)
 add_test(NAME ${TestName} COMMAND SimpleTests ${TestName})
endforeach()