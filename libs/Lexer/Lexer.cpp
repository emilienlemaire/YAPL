#include "Lexer/Lexer.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "CppLogger2/include/Format.h"
#include "TokenUtils.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <string>

std::ostream& operator<<(std::ostream& os, const Token& token){
    os << "Token: " << tokenToString(token.token) << " / Identifier: " << token.identifier;
    return os;
}

Lexer::Lexer(const std::string& filepath)
    :logger(CppLogger::Level::Trace, "Lexer")
{

    CppLogger::Format format({
            CppLogger::FormatAttribute::Name,
            CppLogger::FormatAttribute::Message
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
    std::string identifier;
    std::string numVal;

    if (m_CurrentChar == '\0') {
        getNextChar();
    }

    while (std::isspace(m_CurrentChar)) {
        getNextChar();
    }

    if(m_CurrentChar == EOF) {
        return {token::eof, ""};
    }

    if (std::isalpha(m_CurrentChar)) {
        identifier = m_CurrentChar;
        getNextChar();

        while (std::isalnum(m_CurrentChar)) {
            identifier += m_CurrentChar;
            getNextChar();
        }

        if(identifier == "int") {
            return {token::type, "int"};
        }

        if (identifier == "float" || identifier == "double") {
            return {token::type, "double"};
        }

        if (identifier == "void") {
            return {token::type, "void"};
        }

        if (identifier == "bool") {
            return {token::type, "bool"};
        }

        if (identifier == "string") {
            return {token::type, "string"};
        }

        if (identifier == "func") {
            return {token::func, ""};
        }

        if (identifier == "for") {
            return {token::forlabel, ""};
        }

        if (identifier == "while") {
            return {token::whilelabel, ""};
        }

        if (identifier == "if") {
            return {token::iflabel, ""};
        }

        if (identifier == "else") {
            return {token::elselabel, ""};
        }

        if (identifier == "in") {
            return {token::inlabel, ""};
        }

        if (identifier == "true") {
            return {token::truelabel, ""};
        }

        if (identifier == "false") {
            return {token::falselabel, ""};
        }

        if (identifier == "import") {
            return {token::importlabel, ""};
        }

        if (identifier == "export") {
            return {token::exportlalbel, ""};
        }

        return {token::label, identifier};
    }

    if (std::isdigit(m_CurrentChar)) {
        bool isFloat = false;
        numVal += m_CurrentChar;
        while (std::isdigit(getNextChar()) || (!isFloat && m_CurrentChar == '.')) {
            if (m_CurrentChar == '.') {
                isFloat = true;
            }
            numVal += m_CurrentChar;
        }

        return isFloat ? Token{token::float_value, numVal} :
            Token{token::int_value, numVal};
    }

    if (std::ispunct(m_CurrentChar)) {
        identifier = m_CurrentChar;

        getNextChar();

        while (std::ispunct(m_CurrentChar)) {
            identifier += m_CurrentChar;
            getNextChar();
        }

        if (identifier  == "(") {
            return {token::paropen, ""};
        }

        if (identifier == ")" ) {
            return {token::parclose, ""};
        }

        if (identifier == "{") {
            return {token::bopen, ""};
        }

        if (identifier == "}") {
            return {token::bclose, ""};
        }

        if (identifier == "[") {
            return {token::iopen, ""};
        }

        if (identifier == "]") {
            return {token::iclose, ""};
        }

        if (identifier == "...") {
            return {token::fto, ""};
        }

        if (identifier == "..<") {
            return {token::ftol, ""};
        }

        if (identifier == "..-") {
            return {token::ftom, ""};
        }

        if (identifier == "<..") {
            return {token::ffroml, ""};
        }

        if (identifier == "=") {
            return {token::eq, ""};
        }

        if (identifier == "+") {
            return {token::plus, ""};
        }

        if (identifier == "-") {
            return {token::minus, ""};
        }

        if (identifier == "/") {
            return {token::divide, ""};
        }

        if (identifier == "*") {
            return {token::times};
        }

        if (identifier == "%") {
            return {token::mod, ""};
        }

        if (identifier == "<") {
            return {token::lth, ""};
        }

        if (identifier == ">") {
            return {token::mth, ""};
        }

        if (identifier == "<=") {
            return {token::leq, ""};
        }

        if (identifier == ">=") {
            return {token::meq, ""};
        }

        if (identifier == "!") {
            return {token::notsym, ""};
        }

        if (identifier == "&&") {
            return {token::andsym, ""};
        }

        if (identifier == "||") {
            return {token::orsym, ""};
        }

        if (identifier == "==") {
            return {token::eqsym, ""};
        }

        if (identifier == "!=") {
            return {token::neqsym, ""};
        }

        if (identifier == "::") {
            return {token::dcoloperator, ""};
        }

        if (identifier == ";") {
            return {token::semicolon, ""};
        }

        if (identifier == ",") {
            return {token::comma, ""};
        }

        if (identifier == "->") {
            return {token::arrow, ""};
        }
    }

    return {m_CurrentChar, ""};
}
