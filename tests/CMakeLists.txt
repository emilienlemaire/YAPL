file(GLOB test_files "yapl/*.yapl")

message(STATUS "Found tests files ${test_files}")

foreach(yaplFile IN LISTS test_files)
    message(STATUS "Creating test target for ${yaplFile}")
    get_filename_component(fileName "${yaplFile}" NAME)
    string(REPLACE ".yapl" "" file "${fileName}")
    set(testTarget "testYaplFile_${file}")
    list(APPEND testTargets "${testTarget}")
    add_custom_target(${testTarget}
        COMMAND yapl "${yaplFile}"
        COMMENT "Running yapl on ${fileName}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests/yapl"
        VERBATIM
        )
endforeach()

add_custom_target(runTestFiles ALL DEPENDS "${testTargets}")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/vendor/Catch2/extras")

find_package(Catch2 REQUIRED)

add_executable(tests
    Lexer/TestTokens.cpp)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain lexer)


include(CTest)
enable_testing()

if(POLICY CMP0110)
  cmake_policy(SET CMP0110 NEW)
endif()

message(STATUS "Module path: ${CMAKE_MODULE_PATH}")
include(ParseAndAddCatchTests)
ParseAndAddCatchTests(tests)

include(CPack)
