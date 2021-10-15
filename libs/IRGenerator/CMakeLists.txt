set(LLVM_DIR /usr/local/lib/cmake/llvm)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake from ${LLVM_DIR} and include dir: ${LLVM_INCLUDE_DIRS}")

include_directories(${LLVM_INCLUDE_DIRS})

llvm_map_components_to_libnames(llvm_libs core support irreader mcjit native codegen
    executionengine interpreter mc nativecodegen)

message(STATUS "Using llvm libs: ${llvm_libs}")

file(GLOB SRC_FILES *.cpp)

add_library(irgenerator STATIC ${SRC_FILES})
target_link_libraries(irgenerator PUBLIC ast symbol ${llvm_libs})
