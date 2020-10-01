configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake test for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if(result)
    message(FATAL_ERROR "Build sted for googletest failed: ${result}")
endif()

set(gtest_test_force_shared_crt OM CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
    EXCLUDE_FROM_ALL)

if(CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()

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
