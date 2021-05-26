cmake_minimum_required(VERSION 3.18)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(CMAKE_C_COMPILER /usr/local/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/local/bin/clang++)
    set(CMAKE_LINKER /usr/local/bin/ld.lld)
    set(CMAKE_RANLIB /usr/local/bin/llvm-ranlib)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld -stdlib=libc++ -D_GLIBCXX_USE_CXX11_ABI=0 -fPIC")
# else()
#     set(CMAKE_C_COMPILER /usr/local/opt/llvm/bin/clang)
#     set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
endif()

project(YAPL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "")
endif()

include_directories(include vendor)
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    include_directories(/usr/local/opt/llvm/include)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(libs)
add_subdirectory(vendor)
add_subdirectory(tests)

add_executable(yapl
    main.cpp)

target_link_libraries(yapl PUBLIC cpplogger parser printer yasa)

