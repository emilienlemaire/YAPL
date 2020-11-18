add_subdirectory(googletest)

include_directories(googletest/include)

macro(package_add_test TESTCMD)
    add_executable(${TESTCMD} ${ARGN})
    target_link_libraries(${TESTCMD} gtest gtest_main)
    string(CONCAT TESTNAME ${TESTCMD} "_test")
    add_test(NAME ${TESTNAME} COMMAND ${TESTCMD})
endmacro(package_add_test)

macro(package_add_test_libs)
    set(options "")
    set(oneValueArgs TESTCMD)
    set(multipleValueArgs LIBRARIES SOURCES)
    cmake_parse_arguments(TEST_LIBS "${options}" "${oneValueArgs}" "${multipleValueArgs}" ${ARGN})

    add_executable(${TEST_LIBS_TESTCMD} ${TEST_LIB_SOURCES})
    target_link_libraries(${TEST_LIBS_TESTCMD} gtest gtest_main ${TEST_LIBS_LIBS})
    string(CONCAT TEST_LIBS_TESTNAME ${TEST_LIBS_TESTCMD} "_test")
    add_test(NAME ${TEST_LIBS_TESTNAME} COMMAND ${TESTCMD})
endmacro(package_add_test_libs)

package_add_test(somme simpleTest.cpp)
