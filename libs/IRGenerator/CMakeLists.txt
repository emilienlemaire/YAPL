find_package(LLVM 11.1.0 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    execute_process(COMMAND llvm-config --libs OUTPUT_VARIABLE ld_flags)
    string(REGEX REPLACE "\n$" "" ld_flags "${ld_flags}")
    set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${ld_flags} -L/usr/local/lib")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ld_flags} -L/usr/local/lib")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ld_flags} -lc++abi -L/usr/local/lib")
endif()

include_directories(${LLVM_INCLUDE_DIRS})
# add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs core support irreader mcjit native codegen executionengine
    interpreter mc nativecodegen)

message(STATUS "Found llvmLibs: ${llvm_libs}")

add_library(irgenerator STATIC IRGenerator.cpp Scope.cpp YAPLContext.cpp YAPLValue.cpp)
target_link_libraries(irgenerator PUBLIC ast parser ${llvm_libs} )
