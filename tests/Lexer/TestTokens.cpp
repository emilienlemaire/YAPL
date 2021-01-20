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

TEST_CASE("Can lex identifier", "[lexer][identifier]") {
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

TEST_CASE("Can lex punctuation", "[lexer][punctuation]") {
    SECTION("opening parenthesis") {

    }

    SECTION("closing parenthesis") {

    }

    SECTION("opening curly bracket") {

    }

    SECTION("closing curly bracket") {

    }

    SECTION("opening square braket") {

    }

    SECTION("closing square bracket") {

    }

    // semi_colon
    // comma
    // point
    // colon
    // squote
    // dquote
    // access_sym
}

TEST_CASE("Can lex operators", "[lexer][operators]") {

}

TEST_CASE("Can lex literals", "[lexer][literals]") {

}

TEST_CASE("Can lex keywords", "[lexer][keywords]") {

}

