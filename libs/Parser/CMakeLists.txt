add_library(parser Parser.cpp)

target_link_libraries(parser PUBLIC ast lexer cpplogger)
