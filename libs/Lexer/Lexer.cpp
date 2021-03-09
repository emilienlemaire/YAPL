/**
 * libs/Lexer/Lexer.cpp
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
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <string>

#include "Lexer/Lexer.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "CppLogger2/include/Format.h"
#include "Lexer/TokenUtils.hpp"

std::ostream& operator<<(std::ostream& os, const Position& pos){
    os << pos.line << ":" << pos.column;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Token& token){
    os << "Token: " << tokToString(token.token) << " / Identifier: " << token.identifier;
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
            m_Logger.printError("Cannot open file: {}\n", filepath);
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
        m_CurrentToken = {token::EOF_, "", m_Pos};
        return m_CurrentToken;
    }

    if (std::isalpha(m_CurrentChar)) {
        identifier = (char)m_CurrentChar;
        getNextChar();

        while (std::isalnum(m_CurrentChar) || m_CurrentChar == '_') {
            identifier += (char)m_CurrentChar;
            getNextChar();
        }

        if (identifier == "struct") {
            m_CurrentToken = {token::STRUCT, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "func") {
            m_CurrentToken = {token::FUNC, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "for") {
            m_CurrentToken = {token::FOR, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "while") {
            m_CurrentToken = {token::WHILE, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "if") {
            m_CurrentToken = {token::IF, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "else") {
            m_CurrentToken = {token::ELSE, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "in") {
            m_CurrentToken = {token::IN, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "true") {
            m_CurrentToken = {token::TRUE, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "false") {
            m_CurrentToken = {token::FALSE, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "import") {
            m_CurrentToken = {token::IMPORT, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "export") {
            m_CurrentToken = {token::EXPORT, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "return") {
            m_CurrentToken = {token::RETURN, identifier, m_Pos};
            return m_CurrentToken;
        }
        m_CurrentToken = {token::IDENT, identifier, m_Pos};
        return m_CurrentToken;
    }

    if (std::isdigit(m_CurrentChar)) {
        numVal += (char)m_CurrentChar;
        getNextChar();
        while (std::isdigit(m_CurrentChar)) {
            numVal += (char)m_CurrentChar;
            getNextChar();
        }

        if (m_CurrentChar == 'd') {
            getNextChar();
            m_CurrentToken = {token::DOUBLE_LIT, numVal, m_Pos};
            return m_CurrentToken;
        }

        if (m_CurrentChar == 'f') {
            getNextChar();
            m_CurrentToken = {token::FLOAT_LIT, numVal, m_Pos};
            return m_CurrentToken;
        }

        if (m_CurrentChar == '.') {
            getNextChar();
            numVal += '.';

            while (std::isdigit(m_CurrentChar)) {
                numVal += m_CurrentChar;
                getNextChar();
            }

            if (m_CurrentChar == 'd') {
                getNextChar();
                m_CurrentToken = {token::DOUBLE_LIT, numVal, m_Pos};
                return m_CurrentToken;
            }

            if (m_CurrentChar == 'f') {
                getNextChar();
                m_CurrentToken = {token::FLOAT_LIT, numVal, m_Pos};
                return m_CurrentToken;
            }

            m_CurrentToken = {token::DOUBLE_LIT, numVal, m_Pos};
            return m_CurrentToken;
        }

        m_CurrentToken = {token::INT_LIT, numVal, m_Pos};
        return m_CurrentToken;
    }

    if (std::ispunct(m_CurrentChar)) {
        identifier = (char)m_CurrentChar;

        getNextChar();

        if (identifier  == "(") {
            m_CurrentToken = {token::PAR_O, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == ")" ) {
            m_CurrentToken = {token::PAR_C, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "{") {
            m_CurrentToken = {token::BRA_O, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "}") {
            m_CurrentToken = {token::BRA_C, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "[") {
            m_CurrentToken = {token::ACC_O, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "]") {
            m_CurrentToken = {token::ACC_C, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "=") {
            if (m_CurrentChar == '=') {
                getNextChar();
                m_CurrentToken = {token::EQ, identifier, m_Pos};
                return m_CurrentToken;
            }
            m_CurrentToken = {token::ASSIGN, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "+") {
            m_CurrentToken = {token::PLUS, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "-") {
            if (m_CurrentChar == '>') {
                getNextChar();
                m_CurrentToken = {token::ARROW, identifier, m_Pos};
                return m_CurrentToken;

            }
            m_CurrentToken = {token::MINUS, identifier, m_Pos};
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
            m_CurrentToken = {token::BY, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "*") {
            m_CurrentToken = {token::TIMES, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "%") {
            m_CurrentToken = {token::MOD, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "<") {
            if (m_CurrentChar == '=') {
                getNextChar();
                identifier += "=";
                m_CurrentToken = {token::LEQ, identifier, m_Pos};
                return m_CurrentToken;
            }

            m_CurrentToken = {token::LTH, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == ">") {
            if (m_CurrentChar == '=') {
                getNextChar();
                identifier += "=";
                m_CurrentToken = {token::MEQ, identifier, m_Pos};
                return m_CurrentToken;
            }
            m_CurrentToken = {token::MTH, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "!") {
            if (m_CurrentChar == '=') {
                getNextChar();
                identifier += "=";
                m_CurrentToken = {token::NEQ, identifier, m_Pos};
                return m_CurrentToken;
            }
            m_Logger.printWarn("Unary operation not yet supported please avoid using them."
                    "At position: {}:{}", m_Pos.line, m_Pos.column);
            m_CurrentToken = {token::NOT, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == ";") {
            m_CurrentToken = {token::SEMI, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == ",") {
            m_CurrentToken = {token::COMMA, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == ".") {
            if (m_CurrentChar == '.') {
                identifier += ".";
                getNextChar();
                if (m_CurrentChar == '.') {
                    identifier += ".";
                    getNextChar();
                    m_CurrentToken = {token::FROM_TO, identifier, m_Pos};
                    return m_CurrentToken;
                }
                identifier += (char)m_CurrentChar;
                return {token::NONE, identifier, m_Pos};
            }

            if (std::isdigit(m_CurrentChar)) {
                numVal += '.';

                while (std::isdigit(m_CurrentChar)) {
                    numVal += m_CurrentChar;
                    getNextChar();
                }

                if (m_CurrentChar == 'd') {
                    getNextChar();
                    m_CurrentToken = {token::DOUBLE_LIT, numVal, m_Pos};
                    return m_CurrentToken;
                }

                if (m_CurrentChar == 'f') {
                    getNextChar();
                    m_CurrentToken = {token::FLOAT_LIT, numVal, m_Pos};
                    return m_CurrentToken;
                }

                m_CurrentToken = {token::DOUBLE_LIT, numVal, m_Pos};
                return m_CurrentToken;
            }

            m_CurrentToken = {token::DOT, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == ":") {
            if (m_CurrentChar == ':') {
                identifier += ":";
                getNextChar();
                m_CurrentToken = {token::D_COLON, identifier, m_Pos};
                return m_CurrentToken;
            }
            m_CurrentToken = {token::COLON, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "|") {
            m_CurrentToken = {token::OR, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "&") {
            m_CurrentToken = {token::AND, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "\"") {
            m_CurrentToken = {token::D_QUOTE, identifier, m_Pos};
            return m_CurrentToken;
        }

        if (identifier == "'") {
            m_CurrentToken = {token::S_QUOTE, identifier, m_Pos};
            return m_CurrentToken;
        }
    }

    m_CurrentToken = {token::NONE, identifier, m_Pos};
    return m_CurrentToken;
}

