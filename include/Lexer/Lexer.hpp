#pragma once

#include "Lexer/TokenUtils.hpp"
#include <ostream>
#include <string>
#include <cstdio>

#include <CppLogger2/CppLogger2.h>

struct Position {
    uint32_t line;
    uint32_t column;
    uint32_t character;

    friend std::ostream& operator<<(std::ostream& os, const Position& token);
};

struct Token{
    int token;
    std::string identifier;
    Position pos;

    friend std::ostream& operator<<(std::ostream& os, const Token& token);
    bool operator!=(int tok){
        return token != tok;
    }

    const bool operator==(int tok) const {
        return token == tok;
    }

    const bool operator==(const Token other) const {
        return (this->token == other.token) && (this->identifier == other.identifier);
    }
};

class Lexer
{
private:
    Token m_CurrentToken = {token::unknown, ""};
    int m_CurrentChar = '\0';
    std::string m_CurrentIdentifier = "";
    CppLogger::CppLogger m_Logger;
    Position m_Pos;

    FILE* pFile;

public:
    Lexer(const std::string& filepath="");
    Lexer(FILE* file);
    ~Lexer();

    Token peekToken();
    [[nodiscard]] Token getNextToken();
    int getNextChar();
    [[nodiscard]] const Position getCurrentPos() const { return m_Pos; }
};

