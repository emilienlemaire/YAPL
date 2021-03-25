/**
 * include/Lexer/Lexer.hpp
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
    token token;
    std::string identifier;
    Position pos;

    friend std::ostream& operator<<(std::ostream& os, const Token& token);
    bool operator!=(enum token tok){
        return token != tok;
    }

    const bool operator==(enum token tok) const {
        return token == tok;
    }

    const bool operator==(const Token other) const {
        return (this->token == other.token) && (this->identifier == other.identifier);
    }
};

class Lexer
{
private:
    Token m_CurrentToken = {token::NONE, ""};
    int m_CurrentChar = '\0';
    std::string m_CurrentIdentifier = "";
    CppLogger::CppLogger m_Logger;
    Position m_Pos;

    FILE* pFile;
    std::string lexString();

public:
    Lexer(const std::string& filepath="");
    Lexer(FILE* file);
    ~Lexer();

    Token peekToken();
    [[nodiscard]] Token getNextToken();
    int getNextChar();
    [[nodiscard]] const Position getCurrentPos() const { return m_Pos; }
};

