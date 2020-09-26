add_library(irgenerator IRGenerator.cpp)

target_link_libraries(irgenerator PRIVATE ast parser)
