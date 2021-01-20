#include "Lexer/Lexer.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "CppLogger2/include/Format.h"
#include "Lexer/TokenUtils.hpp"

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
    :m_Logger(CppLogger::Level::Trace, "Lexer")
{

    CppLogger::Format format({
            CppLogger::FormatAttribute::Name,
            CppLogger::FormatAttribute::Message
    });

    m_Logger.setFormat(format);

    if (!filepath.empty()) {
        pFile = fopen(filepath.c_str(), "r");
        if (pFile == nullptr) {
            m_Logger.printError("Cannot open file: {}\nExiting!!", filepath);
            exit(EXIT_FAILURE);
        }
    } else {
        pFile = stdin;
    }
}

Lexer::Lexer(FILE* file)
    :m_Logger(CppLogger::Level::Trace, "Lexer")
{

    CppLogger::Format format({
            CppLogger::FormatAttribute::Name,
            CppLogger::FormatAttribute::Message
    });

    m_Logger.setFormat(format);

    pFile = file;
}
Lexer::~Lexer(){
    fclose(pFile);
}

Token Lexer::peekToken() {
    return m_CurrentToken;
}

int Lexer::getNextChar(){
    m_CurrentChar = getc(pFile);
    if (m_CurrentChar == '\n') {
        m_Pos.line++;
        m_Pos.column = 0;
    } else {
        m_Pos.column++;
    }
    m_Pos.character++;
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
        m_CurrentToken = {token::eof, ""};
        return m_CurrentToken;
    }

    if (std::isalpha(m_CurrentChar)) {
        identifier = (char)m_CurrentChar;
        getNextChar();

        while (std::isalnum(m_CurrentChar) || m_CurrentChar == '_') {
            identifier += (char)m_CurrentChar;
            getNextChar();
        }

        if(identifier == "int") {
            m_CurrentToken = {token::type, "int"};
            return m_CurrentToken;
        }

        if (identifier == "float" || identifier == "double") {
            m_CurrentToken = {token::type, "double"};
            return m_CurrentToken;
        }

        if (identifier == "void") {
            m_CurrentToken = {token::type, "void"};
            return m_CurrentToken;
        }

        if (identifier == "bool") {
            m_CurrentToken = {token::type, "bool"};
            return m_CurrentToken;
        }

        if (identifier == "string") {
            m_CurrentToken = {token::type, "string"};
            return m_CurrentToken;
        }

        if (identifier == "struct") {
            m_CurrentToken = {token::structlabel, ""};
            return m_CurrentToken;
        }

        if (identifier == "func") {
            m_CurrentToken = {token::func, ""};
            return m_CurrentToken;
        }

        if (identifier == "for") {
            m_CurrentToken = {token::forlabel, ""};
            return m_CurrentToken;
        }

        if (identifier == "while") {
            m_CurrentToken = {token::whilelabel, ""};
            return m_CurrentToken;
        }

        if (identifier == "if") {
            m_CurrentToken = {token::iflabel, ""};
            return m_CurrentToken;
        }

        if (identifier == "else") {
            m_CurrentToken = {token::elselabel, ""};
            return m_CurrentToken;
        }

        if (identifier == "in") {
            m_CurrentToken = {token::inlabel, ""};
            return m_CurrentToken;
        }

        if (identifier == "true") {
            m_CurrentToken = {token::truelabel, ""};
            return m_CurrentToken;
        }

        if (identifier == "false") {
            m_CurrentToken = {token::falselabel, ""};
            return m_CurrentToken;
        }

        if (identifier == "import") {
            m_CurrentToken = {token::importlabel, ""};
            return m_CurrentToken;
        }

        if (identifier == "export") {
            m_CurrentToken = {token::exportlalbel, ""};
            return m_CurrentToken;
        }

        if (identifier == "return") {
            m_CurrentToken = {token::returnlabel, ""};
            return m_CurrentToken;
        }
        m_CurrentToken = {token::identifier, identifier};
        return m_CurrentToken;
    }

    if (std::isdigit(m_CurrentChar)) {
        bool isFloat = false;
        numVal += (char)m_CurrentChar;
        getNextChar();
        while (std::isdigit(m_CurrentChar) || (!isFloat && m_CurrentChar == '.')) {
            if (m_CurrentChar == '.') {
                isFloat = true;
            }
            numVal += (char)m_CurrentChar;
            getNextChar();
        }

        m_CurrentToken = isFloat ? Token{token::float_value, numVal} :
            Token{token::int_value, numVal};

        return m_CurrentToken;
    }

    if (std::ispunct(m_CurrentChar)) {
        identifier = (char)m_CurrentChar;

        getNextChar();

        if (identifier  == "(") {
            m_CurrentToken = {token::paropen, ""};
            return m_CurrentToken;
        }

        if (identifier == ")" ) {
            m_CurrentToken = {token::parclose, ""};
            return m_CurrentToken;
        }

        if (identifier == "{") {
            m_CurrentToken = {token::bopen, ""};
            return m_CurrentToken;
        }

        if (identifier == "}") {
            m_CurrentToken = {token::bclose, ""};
            return m_CurrentToken;
        }

        if (identifier == "[") {
            m_CurrentToken = {token::iopen, ""};
            return m_CurrentToken;
        }

        if (identifier == "]") {
            m_CurrentToken = {token::iclose, ""};
            return m_CurrentToken;
        }

        if (identifier == "=") {
            if (m_CurrentChar == '=') {
                getNextChar();
                m_CurrentToken = {token::eqcomp, ""};
                return m_CurrentToken;
            }
            m_CurrentToken = {token::eq, ""};
            return m_CurrentToken;
        }

        if (identifier == "+") {
            m_CurrentToken = {token::plus, ""};
            return m_CurrentToken;
        }

        if (identifier == "-") {
            if (m_CurrentChar == '>') {
                getNextChar();
                m_CurrentToken = {token::arrow_op, ""};
                return m_CurrentToken;
            } else if (std::isdigit(m_CurrentChar)) {
                bool isFloat = false;
                std::string numVal = "-";
                numVal += (char)m_CurrentChar;
                getNextChar();
                while (std::isdigit(m_CurrentChar) || (!isFloat && m_CurrentChar == '.')) {
                    numVal += (char)m_CurrentChar;
                    if (m_CurrentChar == '.') {
                        isFloat = true;
                    }
                    getNextChar();
                }
                m_CurrentToken = {isFloat ? token::float_value : token:: int_value, numVal};
                return m_CurrentToken;
            }
            m_CurrentToken = {token::minus, ""};
            return m_CurrentToken;
        }

        if (identifier == "/") {
            if (m_CurrentChar == '/') {
                getNextChar();
                while (m_CurrentChar != '\n') {
                    getNextChar();
                }
                return getNextToken();
            }
            if (m_CurrentChar == '*') {
                getNextChar();
                while (true) {
                    if (m_CurrentChar == '*') {
                        getNextChar();
                        if (m_CurrentChar == '/') {
                            return getNextToken();
                        }
                    }
                    getNextChar();
                }
            }
            m_CurrentToken = {token::divide, ""};
            return m_CurrentToken;
        }

        if (identifier == "*") {
            m_CurrentToken = {token::times};
            return m_CurrentToken;
        }

        if (identifier == "%") {
            m_CurrentToken = {token::mod, ""};
            return m_CurrentToken;
        }

        if (identifier == "<") {
            if (m_CurrentChar == '=') {
                getNextChar();
                m_CurrentToken = {token::leq, ""};
                return m_CurrentToken;
            } else if (m_CurrentChar == '.') {
                getNextChar();
                if (m_CurrentChar == '.') {
                    getNextChar();
                    m_CurrentToken = {token::fromoreto, ""};
                    return m_CurrentToken;
                }
                identifier += ".";
                identifier += (char)m_CurrentChar;
                m_CurrentToken = {token::unknown, identifier};
                return m_CurrentToken;
            }
            m_CurrentToken = {token::lth, ""};
            return m_CurrentToken;
        }

        if (identifier == ">") {
            if (m_CurrentChar == '=') {
                getNextChar();
                m_CurrentToken = {token::meq, ""};
                return m_CurrentToken;
            }
            m_CurrentToken = {token::mth, ""};
            return m_CurrentToken;
        }

        if (identifier == "!") {
            if (m_CurrentChar == '=') {
                getNextChar();
                m_CurrentToken = {token::neq, ""};
                return m_CurrentToken;
            }
            m_Logger.printWarn("Unary operation not yet supported please avoid using them."
                    "At position: {}:{}", m_Pos.line, m_Pos.column);
            m_CurrentToken = {token::notsym, ""};
            return m_CurrentToken;
        }

        if (identifier == ";") {
            m_CurrentToken = {token::semicolon, ""};
            return m_CurrentToken;
        }

        if (identifier == ",") {
            m_CurrentToken = {token::comma, ""};
            return m_CurrentToken;
        }

        if (identifier == ".") {
            if (std::isdigit(m_CurrentChar)) {
                std::string numVal = ".";
                numVal += (char)m_CurrentChar;
                getNextChar();
                while (std::isdigit(m_CurrentChar)) {
                    numVal += (char)m_CurrentChar;
                    getNextChar();
                }
                m_CurrentToken = {token::float_value, numVal};
                return m_CurrentToken;
            }
            if (m_CurrentChar == '.') {
                getNextChar();
                if (m_CurrentChar == '.') {
                    getNextChar();
                    m_CurrentToken = {token::fromto, ""};
                    return m_CurrentToken;
                }
                if (m_CurrentChar == '<') {
                    getNextChar();
                    m_CurrentToken = {token::fromtol, ""};
                    return m_CurrentToken;
                }
                if (m_CurrentChar == '-') {
                    getNextChar();
                    m_CurrentToken = {token::fromtominus, ""};
                    return m_CurrentToken;
                }
                identifier += ".";
                identifier += (char)m_CurrentChar;
                return {token::unknown, identifier};
            }
            m_CurrentToken = {token::point, ""};
            return m_CurrentToken;
        }

        if (identifier == ":") {
            if (m_CurrentChar == ':') {
                getNextChar();
                m_CurrentToken = {token::access_sym, ""};
                return m_CurrentToken;
            }
            m_CurrentToken = {token::colon, ""};
            return m_CurrentToken;
        }

        if (identifier == "|") {
            m_CurrentToken = {token::orsym, ""};
            return m_CurrentToken;
        }

        if (identifier == "&") {
            m_CurrentToken = {token::andsym, ""};
            return m_CurrentToken;
        }

        if (identifier == "\"") {
            m_CurrentToken = {token::dquote, ""};
            return m_CurrentToken;
        }

        if (identifier == "'") {
            m_CurrentToken = {token::squote, ""};
            return m_CurrentToken;
        }
    }

    m_CurrentToken = {m_CurrentChar, ""};
    return m_CurrentToken;
}

