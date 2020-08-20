#include "Lexer/Lexer.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "CppLogger2/include/Format.h"
#include "TokenUtils.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <ostream>

std::ostream& operator<<(std::ostream& os, const Token& token){
    os << "Token: " << tokenToString(token.token) << " / Identifier: " << token.identifier;
    return os;
}

Lexer::Lexer(const std::string& filepath)
    :logger(CppLogger::Level::Trace, "Lexer")
{

    CppLogger::Format format({
            CppLogger::FormatAttribute::Name
    });

    logger.setFormat(format);

    if (filepath != "") {
        pFile = fopen(filepath.c_str(), "r");
        if (pFile == nullptr) {
            logger.printError("Cannot open file: {}\nExiting!!", filepath);
            exit(EXIT_FAILURE);
        }
    } else {
        pFile = stdin;
    }
}

Lexer::~Lexer(){
    fclose(pFile);
}

int Lexer::getNextChar(){
    m_CurrentChar = getc(pFile);

    return m_CurrentChar;
}

Token Lexer::getNextToken(){

    getNextChar();

    std::string identifier;
    double numVal;

    while (std::isspace(m_CurrentChar)) {
        getNextChar();
    }

    if(m_CurrentChar == EOF) {
        return {token::eof, ""};
    }
    
    if (std::isalpha(m_CurrentChar)) {
        identifier = m_CurrentChar;
        while (std::isalnum(getNextChar())) {
            identifier += m_CurrentChar;
        }
        
        if(identifier == "int") {
            return {token::type, "int"};
        }

        if (identifier == "def") {
            return {token::def, ""};
        }

        if (identifier == "float" || identifier == "double") {
            return {token::type, "double"};
        }

        return {token::label, identifier};
    }

    return {m_CurrentChar, ""};
}
