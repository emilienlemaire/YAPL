add_library(compiler SHARED Compiler.cpp)
target_link_libraries(compiler PUBLIC irgenerator)
