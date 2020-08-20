#ifndef LEXER_H
#define LEXER_H

#include <ostream>
#include <string>
#include <cstdio>
#include <string_view>

#include <CppLogger2/CppLogger2.h>

struct Token{
    int token;
    std::string identifier;

    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

class Lexer
{
private:
    Token m_CurrentToken;
    int m_CurrentChar = '\0';
    std::string m_CurrentIdentifier = "";
    CppLogger::CppLogger logger;

    FILE* pFile;

public:
    Lexer(const std::string& filepath="");
    ~Lexer();

    Token peekToken();
    [[nodiscard]] Token getNextToken();
    int getNextChar();
};


#endif /* LEXER_H */

