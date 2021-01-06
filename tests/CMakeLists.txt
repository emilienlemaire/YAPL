enable_testing()

set($ENV{CTEST_OUTPUT_ON_FAILURE} 1)

add_subdirectory(catch2)

include_directories(catch2)

macro(package_add_test TESTCMD)
    add_executable(${TESTCMD} ${ARGN})
    target_link_libraries(${TESTCMD} Catch2::Catch2)
    add_test(NAME ${TESTCMD} COMMAND ${TESTCMD})
endmacro(package_add_test)

macro(package_add_test_libs)
    set(options "")
    set(oneValueArgs COMMAND)
    set(multipleValueArgs LIBRARY SOURCES)
    cmake_parse_arguments(TEST_LIBS "${options}" "${oneValueArgs}" "${multipleValueArgs}" ${ARGN})

    add_executable(${TEST_LIBS_COMMAND} ${TEST_LIBS_SOURCES})
    target_link_libraries(${TEST_LIBS_COMMAND} catch2 ${TEST_LIBS_LIBRARY})
    #string(CONCAT TEST_LIBS_TESTNAME ${TEST_LIBS_COMMAND} "_test")
    add_test(NAME ${TEST_LIBS_COMMAND} COMMAND ${TEST_LIBS_COMMAND})
endmacro(package_add_test_libs)

add_executable(all_tests
    main_tests.cpp
    Lexer/TestTokens.cpp)

target_link_libraries(all_tests catch2 lexer)

