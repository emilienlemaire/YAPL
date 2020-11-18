include_directories(include)

add_library(gtest src/gtest-all.cc)
add_library(gtest_main src/gtest_main.cc)

target_link_libraries(gtest PRIVATE pthread)
target_link_libraries(gtest_main PRIVATE pthread)
