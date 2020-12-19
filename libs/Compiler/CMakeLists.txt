add_library(compiler STATIC Compiler.cpp)
target_link_libraries(compiler PUBLIC irgenerator)
