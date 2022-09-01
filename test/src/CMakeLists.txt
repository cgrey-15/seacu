cmake_minimum_required(VERSION 3.19)

set(TEST_SOURCES MatchingAndSimpleSeq.cpp LightlyComplexSeq.cpp SimpleLines.cpp)

create_test_sourcelist(Tests SimpleTests.cpp ${TEST_SOURCES})

add_executable(SimpleTests ${Tests})

target_link_libraries(SimpleTests PRIVATE seacudiff Boost::headers)

set(TestsToRun ${TEST_SOURCES})

foreach(test ${TestsToRun})
 get_filename_component(TestName ${test} NAME_WE)
 add_test(NAME ${TestName} COMMAND SimpleTests ${TestName})
endforeach()
