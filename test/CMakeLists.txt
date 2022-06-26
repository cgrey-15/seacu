cmake_minimum_required(VERSION 3.19)

 enable_testing()
if(CMAKE_CXX_COMPILER_ID MATCHES "[Cc][Ll][Aa][Nn][Gg]" OR CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
 add_subdirectory(src)
else()
 # CRASH TESTS
 add_test(NAME test01 COMMAND test_prog -i hi h)
 add_test(NAME test02 COMMAND test_prog -i a a)
 add_test(NAME test03 COMMAND test_prog -i abcd abcd)
 add_test(NAME test04 COMMAND test_prog -i abcabba cbabac)

 add_test(NAME test05 COMMAND test_prog -i abc ab)
 add_test(NAME test06 COMMAND test_prog -i abcde ab)
 add_test(NAME test07 COMMAND test_prog -i thisarecown t)
 add_test(NAME test08 COMMAND test_prog -i thisarecown th)
 add_test(NAME test09 COMMAND test_prog -i ThisAreCown ThisAreCownB)
 add_test(NAME test10 COMMAND test_prog -i whatisdamenupofyk whatisthemean)

 add_test(NAME test11 COMMAND test_prog -i abcdefghijklmnopqrs abdefghjkqrABs)
 add_test(NAME test12 COMMAND test_prog -i ABCabcdefghijklm ABCdegno)
endif()
