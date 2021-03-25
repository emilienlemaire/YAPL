/**
 * tests/Lexer/TestTokens.cpp
 * Copyright (c) 2021 Emilien Lemaire <emilien.lem@icloud.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
    // access_sym
}

TEST_CASE("Can lex operators", "[lexer][operators]") {

}

TEST_CASE("Can lex literals", "[lexer][literals]") {
    SECTION("empty string literal") {
        generateFile("empty_string_lit.yapl", "\"\"");
        auto lexer = Lexer("empty_string_lit.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::STRING_LIT, ""}));
        remove("empty_string_lit.yapl");
    }

    SECTION("string literal") {
        generateFile("string_lit.yapl", "\"test\"");
        auto lexer = Lexer("string_lit.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::STRING_LIT, "test"}));
        remove("string_lit.yapl");
    }

    SECTION("escaped string literals") {
        generateFile("escaped_string_lit.yapl", "\"this is an \\\"escaped\\\" string lit\"");
        auto lexer = Lexer("escaped_string_lit.yapl");
        REQUIRE(lexer.getNextToken() == (Token{token::STRING_LIT, "this is an \"escaped\" string lit"}));
        remove("escaped_string_lit.yapl");
    }
}

TEST_CASE("Can lex keywords", "[lexer][keywords]") {

}

