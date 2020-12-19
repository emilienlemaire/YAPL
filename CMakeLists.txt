cmake_minimum_required(VERSION 3.18)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(CMAKE_C_COMPILER /usr/local/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/local/bin/clang++)
    set(CMAKE_LINKER /usr/local/bin/ld.lld)
    set(CMAKE_RANLIB /usr/local/bin/llvm-ranlib)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld -stdlib=libc++ -D_GLIBCXX_USE_CXX11_ABI=0 -fPIC")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "")   
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

string(REPLACE " -Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

project(YAPL)

include(CTest)

include_directories(include vendor)
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    include_directories(/usr/local/opt/llvm/include)
endif()

file(GLOB test_files "tests/yapl/*.yapl")

message(STATUS "Found tests files ${test_files}")

add_subdirectory(libs)
add_subdirectory(vendor)
add_subdirectory(tests)

add_executable(yapl
    main.cpp)


target_link_libraries(yapl PUBLIC cpplogger irgenerator)

add_custom_target(run_YAPL ALL DEPENDS yapl)

foreach(test_file IN LISTS test_files)
    message(STATUS "Adding command for ${test_file}")
    add_custom_command(
        TARGET run_YAPL
        POST_BUILD
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/yapl ${test_file}
        COMMENT "Running test on ${test_file}"
        VERBATIM)
endforeach()


