add_executable(tests
    main_tests.cpp
    Lexer/TestTokens.cpp)

target_link_libraries(tests PRIVATE Catch2::Catch2 lexer)

