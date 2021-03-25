find_package(Catch2 REQUIRED)

add_executable(tests
    Lexer/TestTokens.cpp)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain lexer)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/vendor/Catch2/extras")

include(CTest)
enable_testing()

if(POLICY CMP0110)
  cmake_policy(SET CMP0110 NEW)
endif()

message(STATUS "Module path: ${CMAKE_MODULE_PATH}")
include(ParseAndAddCatchTests)
ParseAndAddCatchTests(tests)

include(CPack)
