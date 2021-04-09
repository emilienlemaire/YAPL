add_library(yasa STATIC Yasa.cpp YasaVisitor.cpp)

target_link_libraries(yasa PRIVATE ast symbol)
