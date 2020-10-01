add_library(parser SHARED Parser.cpp)

target_link_libraries(parser PUBLIC ast lexer cpplogger)
