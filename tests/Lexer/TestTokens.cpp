#include <fstream>
#include <stdio.h>

#include "catch2.hpp"
#include "Lexer/Lexer.hpp"

void generateFile(std::string name, std::string content) {
    std::ofstream file;
    file.open(name);
    file << content;
    file.close();
}

TEST_CASE("Can lex identifier", "[lexer_tokens]") {
    SECTION("with lower case") {
        generateFile("LowerCaseIdentifier.yapl", "lowercaseidentifier");
        auto lexer = Lexer("LowerCaseIdentifier.yapl");
        REQUIRE(lexer.getNextToken() == Token{token::identifier, "lowercaseidentifier"});
        remove("LowerCaseIdentifier.yapl");
    }

    SECTION("with mixed case") {
        generateFile("MixedCaseIdentifier.yapl", "MixedCaseIdentifier");
        auto lexer = Lexer("MixedCaseIdentifier.yapl");
        REQUIRE(lexer.getNextToken() == Token{token::identifier, "MixedCaseIdentifier"});
        remove("MixedCaseIdentifier.yapl");
    }

    SECTION("with underscore") {
        generateFile("UnderscoredIdentifier.yapl", "m_UnderscoredIdentifier");
        auto lexer = Lexer("UnderscoredIdentifier.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::identifier, "m_UnderscoredIdentifier"}));
        remove("UnderscoredIdentifier.yapl");
    }

    SECTION("with numbers") {
        generateFile("NumberedIdentifier.yapl", "m_Numbered0");
        auto lexer = Lexer("NumberedIdentifier.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::identifier, "m_Numbered0"}));
        remove("NumberedIdentifier.yapl");

    }
}
