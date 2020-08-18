#include "Lexer/Lexer.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "CppLogger2/include/Format.h"
#include <cstdio>
#include <cstdlib>
#include <ostream>

std::ostream& operator<<(std::ostream& os, const Token& token){
    os << "Token: " << token.token << " / Identifier: " << token.identifier;
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
