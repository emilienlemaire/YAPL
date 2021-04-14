add_library(printer STATIC ASTPrinter.cpp)

target_link_libraries(printer PRIVATE ast symbol)
