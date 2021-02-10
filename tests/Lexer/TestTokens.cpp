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
        REQUIRE(lexer.getNextToken() == Token{token::IDENT, "lowercaseidentifier"});
        remove("LowerCaseIdentifier.yapl");
    }

    SECTION("with mixed case") {
        generateFile("MixedCaseIdentifier.yapl", "MixedCaseIdentifier");
        auto lexer = Lexer("MixedCaseIdentifier.yapl");
        REQUIRE(lexer.getNextToken() == Token{token::IDENT, "MixedCaseIdentifier"});
        remove("MixedCaseIdentifier.yapl");
    }

    SECTION("with underscore") {
        generateFile("UnderscoredIdentifier.yapl", "m_UnderscoredIdentifier");
        auto lexer = Lexer("UnderscoredIdentifier.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::IDENT, "m_UnderscoredIdentifier"}));
        remove("UnderscoredIdentifier.yapl");
    }

    SECTION("with numbers") {
        generateFile("NumberedIdentifier.yapl", "m_Numbered0");
        auto lexer = Lexer("NumberedIdentifier.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::IDENT, "m_Numbered0"}));
        remove("NumberedIdentifier.yapl");

    }
}

TEST_CASE("Can lex punctuation", "[lexer][punctuation]") {
    SECTION("opening parenthesis") {
        generateFile("openingpar.yapl", "(");
        auto lexer = Lexer("openingpar.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::PAR_O, "("}));
        remove("openingpar.yapl");
    }

    SECTION("closing parenthesis") {
        generateFile("closingpar.yapl", ")");
        auto lexer = Lexer("closingpar.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::PAR_C, ")"}));
        remove("closingpar.yapl");
    }

    SECTION("opening curly bracket") {
        generateFile("openingbra.yapl", "{");
        auto lexer = Lexer("openingbra.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::BRA_O, "{"}));
        remove("openingbra.yapl");
    }

    SECTION("closing curly bracket") {
        generateFile("closingbra.yapl", "}");
        auto lexer = Lexer("closingbra.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::BRA_C, "}"}));
        remove("closingbra.yapl");
    }

    SECTION("opening square braket") {
        generateFile("openingbra.yapl", "[");
        auto lexer = Lexer("openingbra.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::ACC_O, "["}));
        remove("openingbra.yapl");
    }

    SECTION("closing square bracket") {
        generateFile("closingbra.yapl", "]");
        auto lexer = Lexer("closingbra.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::ACC_C, "]"}));
        remove("closingbra.yapl");
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

