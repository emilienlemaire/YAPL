cmake_minimum_required(VERSION 3.18)

set(CMAKE_C_COMPILER /usr/local/bin/clang)
set(CMAKE_CXX_COMPILER /usr/local/bin/clang++)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(YAPL)

include_directories(include vendor)

link_directories(vendor/CppLogger2/Debug/src)
add_subdirectory(libs)

add_executable(YAPL
    main.cpp)

target_link_libraries(YAPL PRIVATE cpplogger lexer parser)
