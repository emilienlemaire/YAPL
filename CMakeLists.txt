cmake_minimum_required(VERSION 3.18)

set(CMAKE_C_COMPILER /usr/local/bin/clang)
set(CMAKE_CXX_COMPILER /usr/local/bin/clang++)
set(CMAKE_LINKER /usr/local/bin/ld.lld)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld -stdlib=libc++ -D_GLIBCXX_USE_CXX11_ABI=0")

string(REPLACE " -Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

project(YAPL)

include(CTest)

include_directories(include vendor)

add_subdirectory(libs)
add_subdirectory(vendor)
add_subdirectory(tests)

add_executable(YAPL
    main.cpp)

target_link_libraries(YAPL PUBLIC cpplogger lexer parser irgenerator)
