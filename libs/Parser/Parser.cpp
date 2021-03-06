/**
 * libs/Parser/Parser.cpp
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
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "Parser/Parser.hpp"
#include "AST/ASTExprNode.hpp"
#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"
#include "CppLogger2/CppLogger2.h"
#include "Lexer/TokenUtils.hpp"

//#define LOG_PARSER

namespace yapl {
    Parser::Parser(const std::string &filepath, CppLogger::Level level)
        : m_Logger(level, "Parser"), m_Lexer(filepath), m_FilePath(filepath)
    {
        CppLogger::Format format({
                CppLogger::FormatAttribute::Name,
                CppLogger::FormatAttribute::Level,
                CppLogger::FormatAttribute::Message
                });

        m_Logger.setFormat(format);

        m_SymbolTable = SymbolTable::InitTopSymTab();
    }

    void Parser::parseInfo(const std::string &info) {
#ifdef LOG_PARSER
        m_Logger.printInfo("Parsing {}", info);
#endif
    }

    int Parser::getOpPrecedence(Operator t_Operator){
        switch (t_Operator) {
            case Operator::TIMES:
            case Operator::BY:
            case Operator::MOD:
                return 5;
            case Operator::PLUS:
            case Operator::MINUS:
                return 6;
            case Operator::LTH:
            case Operator::MTH:
            case Operator::LEQ:
            case Operator::MEQ:
                return 9;
            case Operator::EQ:
            case Operator::NEQ:
                return 10;
            case Operator::AND:
                return 14;
            case Operator::OR:
                return 15;
        }
    }

    void Parser::parse() {
        std::vector<std::unique_ptr<ASTNode>> nodes;
        m_CurrentToken = m_Lexer.getNextToken();

        while (m_Lexer.peekToken().token != token::EOF_) {
            auto node = parseNext();

            if (!node) {
                m_CurrentToken = m_Lexer.getNextToken();
            }

            nodes.push_back(std::move(node));
        }

        m_Program = std::make_unique<ASTProgramNode>(m_SymbolTable, std::move(nodes));
    }

    std::unique_ptr<ASTProgramNode> Parser::getProgram() {
        return std::move(m_Program);
    }

    std::unique_ptr<ASTNode> Parser::parseNext() {
        parseInfo("next");

        while (m_CurrentToken == token::SEMI) {
            m_CurrentToken = m_Lexer.getNextToken();
        }

        if (m_CurrentToken.token == token::IMPORT) {
            return parseImport();
        }

        if (m_CurrentToken == token::EXPORT) {
            return parseExport();
        }

        if (m_CurrentToken == token::FUNC) {
            return parseFunctionDefinition();
        }

        if (m_CurrentToken == token::STRUCT) {
            return parseStructDefinition();
        }

        if (m_CurrentToken == token::IDENT) {
            return parseIdentifier(m_CurrentToken.identifier);
        }

        if (m_CurrentToken == token::EOF_) {
            return std::make_unique<ASTEOFNode>(m_SymbolTable);
        }

        // We can only have sentences that start with the token above at top level.
        return parseError<ASTNode>(
                "File: {}:{}\n\tUnexpected token at top level scope: {}",
                m_FilePath,
                m_CurrentToken.pos,
                m_CurrentToken);
    }

    /* TODO:
     *  - Make it possible to import several values from on namespace
     *    i.e.: import namespace1::namespace2::{value1, value2};
     * */
    std::unique_ptr<ASTImportNode> Parser::parseImport() {
        std::string currentIdentifier;

        std::unique_ptr<ASTImportNode> importNode = std::make_unique<ASTImportNode>(m_SymbolTable);

        m_CurrentToken = m_Lexer.getNextToken();

        if (m_CurrentToken != token::IDENT) {
            return parseError<ASTImportNode>(
                    "File: {}:{}\n\tExpecting an identifier after 'import' instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        currentIdentifier = m_CurrentToken.identifier;

        m_CurrentToken = m_Lexer.getNextToken();

        while (m_CurrentToken == token::D_COLON) {
            importNode->addNamespace(currentIdentifier);
            m_CurrentToken = m_Lexer.getNextToken(); // Eat ::

            if (m_CurrentToken != token::IDENT) {
                return parseError<ASTImportNode>(
                        "File: {}:{}\n\tExpecting an identifier after '::' instead of {}",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

            currentIdentifier = m_CurrentToken.identifier;

            m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier
        }

        importNode->setImportedValue(currentIdentifier);

        if (m_CurrentToken != token::SEMI) {
            return parseError<ASTImportNode>(
                    "File: {}:{}\n\tExpecting a ';' after 'import' statement instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat ';'

        return std::move(importNode);
    }

    std::unique_ptr<ASTExportNode> Parser::parseExport() {
        m_CurrentToken = m_Lexer.getNextToken(); // Eat 'export'

        ASTExportNode exportNode(m_SymbolTable);

        if (m_CurrentToken == token::IDENT) {
            exportNode.addExportedValue(m_CurrentToken.identifier);

            m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier;

            if (m_CurrentToken != token::SEMI) {
                return parseError<ASTExportNode>(
                        "File: {}:{}\n\tExpecting a ';' after export statement, instead of {}",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

            m_CurrentToken = m_Lexer.getNextToken(); // Eat ';'

            return std::make_unique<ASTExportNode>(exportNode);
        }

        if (m_CurrentToken == token::BRA_O) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat '{'

            if (m_CurrentToken == token::IDENT) {
                std::string currentIdentifier = m_CurrentToken.identifier;

                m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier

                while (m_CurrentToken == token::COMMA) {
                    exportNode.addExportedValue(currentIdentifier);

                    m_CurrentToken = m_Lexer.getNextToken(); // Eat ','

                    if (m_CurrentToken != token::IDENT) {
                        return parseError<ASTExportNode>(
                                "File: {}:{}\n\tExpecting an identifier after ',' instead of {}",
                                m_FilePath,
                                m_CurrentToken.pos,
                                m_CurrentToken
                            );
                    }

                    currentIdentifier = m_CurrentToken.identifier;
                    m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier
                } // While token == COMMA

                exportNode.addExportedValue(currentIdentifier);

                if (m_CurrentToken != token::BRA_C) {
                    return parseError<ASTExportNode>(
                            "File: {}:{}\n\tMissing matching '}' instead of {}",
                            m_FilePath,
                            m_CurrentToken.pos,
                            m_CurrentToken
                        );
                }

                m_CurrentToken = m_Lexer.getNextToken(); // Eat '}'

                return std::make_unique<ASTExportNode>(exportNode);
            }

            return parseError<ASTExportNode>(
                    "File: {}:{}\n\tExpecting an identifier after '{' instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        return parseError<ASTExportNode>(
                "File: {}:{}\n\tExpecting an identifier or a '{' after 'export' instead of {}",
                m_FilePath,
                m_CurrentToken.pos,
                m_CurrentToken
            );
    }

    std::unique_ptr<ASTFunctionDefinitionNode> Parser::parseFunctionDefinition() {
        return nullptr;
    }

    std::unique_ptr<ASTStructDefinitionNode> Parser::parseStructDefinition() {
        return nullptr;
    }

    std::unique_ptr<ASTNode> Parser::parseIdentifier(const std::string &identifier) {
        std::string firstIdentifier = identifier;
        m_CurrentToken = m_Lexer.getNextToken();

        if (m_CurrentToken == token::IDENT) {
            return parseDeclaration(identifier);
        }

        return parseError<ASTNode>(
                "File: {}:{}\n\tUnxecpted token after identifier: {}",
                m_FilePath,
                m_CurrentToken.pos,
                m_CurrentToken
            );
    }

    std::unique_ptr<ASTDeclarationNode> Parser::parseDeclaration(const std::string &type) {
        std::unique_ptr<ASTDeclarationNode> declarationNode = std::make_unique<ASTDeclarationNode>(m_SymbolTable);

        declarationNode->setType(type);

        std::string identifier = m_CurrentToken.identifier;

        declarationNode->setIdentifier(identifier);

        // It doesn't matter now if the type value is nullptr, as we will check it
        // during semantic analysis.
        auto typeValue = m_SymbolTable->lookup(type);

        auto variable = Value::CreateVariableValue(identifier, typeValue);

        m_SymbolTable->insert(variable);

        m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier

        /* TODO:
         *  - Initialization
         *  - Array
         * */

        if (m_CurrentToken != token::SEMI){
            return parseError<ASTDeclarationNode>(
                    "File: {}:{}\n\tMissing a ';' after a variable declaration",
                    m_FilePath,
                    m_CurrentToken.pos
                    );
        }

        return std::move(declarationNode);
    }

    std::unique_ptr<ASTExprNode> Parser::parseExpr() {
        // Expressions can start with a parenthesis, a dot, a number, an identifier or an unary operator
        // We first parse an expression and then if the next token is a binary operator we parse it
        // as such.

        std::unique_ptr<ASTExprNode> exprTmp;

        if (m_CurrentToken == token::PAR_O) {
            exprTmp = parseParenExpr();
        }

        if (m_CurrentToken == token::INT_LIT ||
                m_CurrentToken == token::FLOAT_LIT ||
                m_CurrentToken == token::DOUBLE_LIT) {
            exprTmp = parseNumberExpr();
        }

        if (m_CurrentToken == token::IDENT) {
            exprTmp = parseIdentifierExpr();
        }

        if (m_CurrentToken == token::NOT ||
                m_CurrentToken == token::MINUS) {
            exprTmp = parseUnaryExpr();
        }

        // All tokens higher than assign are binray operators.
        if (m_CurrentToken.token > token::ASSIGN) {
            return parseBinaryExpr(std::move(exprTmp));
        }

        return exprTmp;
    }

    std::unique_ptr<ASTExprNode> Parser::parseParenExpr() {
        std::unique_ptr<ASTExprNode> expr = parseExpr();

        if (m_CurrentToken != token::PAR_C) {
            return parseError<ASTExprNode>(
                    "File: {}:{}\n\tMissing matching ')'.",
                    m_FilePath,
                    m_CurrentToken.pos
                );
        }

        return expr;
    }

    std::unique_ptr<ASTNumberExpr> Parser::parseNumberExpr() {
        auto savedTok = m_CurrentToken;

        m_CurrentToken = m_Lexer.getNextToken(); // Eat number

        if (savedTok == token::INT_LIT) {
            auto expr = std::make_unique<ASTIntegerNumberExpr>(m_SymbolTable);

            expr->setValue(std::stoi(savedTok.identifier));

            return expr;
        }

        if (savedTok == token::FLOAT_LIT) {
            auto expr = std::make_unique<ASTFloatNumberExpr>(m_SymbolTable);

            expr->setValue(std::stof(savedTok.identifier));

            return expr;
        }

        if (savedTok == token::DOUBLE_LIT) {
            auto expr = std::make_unique<ASTDoubleNumberExpr>(m_SymbolTable);

            expr->setValue(std::stod(savedTok.identifier));

            return expr;
        }

        assert(false && "Something went very wrong, this shouldn't happen");

        return parseError<ASTNumberExpr>("Something went very wrong, this shouldn't happen");
    }
} // namespace yapl
