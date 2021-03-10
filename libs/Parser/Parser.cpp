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
#include <queue>
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
            case Operator::NONE:
                return -1;
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

            if (m_CurrentToken != token::IDENT && m_CurrentToken != token::BRA_O) {
                return parseError<ASTImportNode>(
                        "File: {}:{}\n\tExpecting an identifier or '{' after '::' instead of {}",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

            if (m_CurrentToken == token::BRA_O) // Cas namespace::{value, value1};
                break;

            currentIdentifier = m_CurrentToken.identifier;

            m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier
        }

        if (m_CurrentToken == token::SEMI) { // Cas import namespace::value;
            importNode->addImportedValue(currentIdentifier);
        }

        if (m_CurrentToken == token::BRA_O) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat '{'

            if (m_CurrentToken != token::IDENT)
            {
                return parseError<ASTImportNode>(
                        "File {}:{}\n\tExpecting an identifier after '{' in import statement instead of {}",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

            while (m_CurrentToken == token::IDENT) {
                importNode->addImportedValue(m_CurrentToken.identifier);
                m_CurrentToken = m_Lexer.getNextToken();

                if (m_CurrentToken != token::COMMA && m_CurrentToken != token::BRA_C) {
                    return parseError<ASTImportNode>(
                            "File: {}:{}\n\tExpecting an identifier or '{' after '::' instead of {}",
                            m_FilePath,
                            m_CurrentToken.pos,
                            m_CurrentToken
                        );
                }

                if (m_CurrentToken == token::BRA_C) {
                    m_CurrentToken = m_Lexer.getNextToken(); // Eat '}'
                    break;
                }

                if (m_CurrentToken == token::COMMA) {
                    m_CurrentToken = m_Lexer.getNextToken(); // Eat ','

                    if (m_CurrentToken != token::IDENT) {
                        return parseError<ASTImportNode>(
                                "File: {}:{}\n\tExpecting an identifier after ',' "
                                "in import statement instead of {}",
                                m_FilePath,
                                m_CurrentToken.pos,
                                m_CurrentToken
                            );
                    }
                }
            }
        }

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
            return parseDeclaration(firstIdentifier);
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

        m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier

        if (m_CurrentToken == token::ACC_O) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat '['

            return parseArrayDeclaration(std::move(declarationNode));
        }

        // It doesn't matter now if the type value is nullptr, as we will check it
        // during semantic analysis.
        auto typeValue = m_SymbolTable->lookup(type);

        auto variable = Value::CreateVariableValue(identifier, typeValue);

        m_SymbolTable->insert(variable);

        if (m_CurrentToken == token::ASSIGN) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat '='

            return parseInitialization(std::move(declarationNode));
        }

        if (m_CurrentToken != token::SEMI){
            return parseError<ASTDeclarationNode>(
                    "File: {}:{}\n\tMissing a ';' after a variable declaration",
                    m_FilePath,
                    m_CurrentToken.pos
                    );
        }

        return std::move(declarationNode);
    }

    std::unique_ptr<ASTInitializationNode> Parser::parseInitialization(
            std::unique_ptr<ASTDeclarationNode> declaration
        ) {
        auto type = declaration->getType();
        auto identifier = declaration->getIdentifier();

        auto initialization = std::make_unique<ASTInitializationNode>(m_SymbolTable);

        if (auto expr = parseExpr()) {
            initialization->setValue(std::move(expr));

            if (m_CurrentToken != token::SEMI) {
                return parseError<ASTInitializationNode>(
                        "File {}:{}\n\tExpecting a ';' after initialization instead of {}",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

            m_CurrentToken = m_Lexer.getNextToken();

            return initialization;
        }

        return parseError<ASTInitializationNode>(
                "File {}:{}\n\tExpecting an expression after '=' instead of {}",
                m_FilePath,
                m_CurrentToken.pos,
                m_CurrentToken
            );
    }

    std::unique_ptr<ASTArrayDeclarationNode> Parser::parseArrayDeclaration(
            std::unique_ptr<ASTDeclarationNode> declaration
            ) {
        if (m_CurrentToken != token::INT_LIT) {
            return parseError<ASTArrayDeclarationNode>(
                    "File {}:{}\n\tExpecting an integer literal afater '[' instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        auto arrayDeclaration = std::make_unique<ASTArrayDeclarationNode>(m_SymbolTable);

        auto typeName = declaration->getType();

        int size = std::stoi(m_CurrentToken.identifier);

        auto typeValue = m_SymbolTable->lookup(typeName);

        if (!typeValue) {
            return parseError<ASTArrayDeclarationNode>(
                    "File: {}:{}\n\tTrying to declare an array of unknown type: {}",
                    m_FilePath,
                    m_CurrentToken,
                    typeName
                    );
        }

        auto type = typeValue->getType();

        if (auto arrDecl = dynamic_cast<ASTArrayDeclarationNode*>(declaration.get())) {
            typeName = Type::MangleArrayType(type, arrDecl->getSize());
            typeValue = m_SymbolTable->lookup(typeName);
            type = typeValue->getType();
        }


        auto mangledType = Type::MangleArrayType(type, size);

        if (!m_SymbolTable->lookup(mangledType)) {
            auto newType = Type::CreateArrayType(size, type);
            auto newTypeValue = Value::CreateTypeValue(mangledType, newType);

            m_SymbolTable->insert(newTypeValue);
        }

        arrayDeclaration->setType(typeName);
        arrayDeclaration->setIdentifier(declaration->getIdentifier());
        arrayDeclaration->setSize(size);

        m_CurrentToken = m_Lexer.getNextToken(); // Eat int literal

        if (m_CurrentToken != token::ACC_C) {
            return parseError<ASTArrayDeclarationNode>(
                    "File: {}:{}\n\tExpecting a ']' after array declaration instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken();

        if (m_CurrentToken == token::ACC_O) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat '['

            return parseArrayDeclaration(std::move(arrayDeclaration));
        }

        if (m_CurrentToken == token::ASSIGN) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat '='

            return parseArrayInitialization(std::move(arrayDeclaration));
        }

        if (m_CurrentToken != token::SEMI) {
            return parseError<ASTArrayDeclarationNode>(
                    "File: {}:{}\n\tExpecting a ';', '[' or '=' after array declaration instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        return arrayDeclaration;
    }

    std::unique_ptr<ASTArrayInitializationNode> Parser::parseArrayInitialization(
            std::unique_ptr<ASTArrayDeclarationNode> declaration
        ) {
        // We dont care if types are incompatible we'll check that later
        auto values = parseExpr();
        if (!values) {
            return parseError<ASTArrayInitializationNode>(
                    "File: {}:{}\n\tExpecting an expression. Got :",
                    m_FilePath,
                    m_CurrentToken.pos
                    );
        }

        auto arrayInitialization = std::make_unique<ASTArrayInitializationNode>(m_SymbolTable);
        arrayInitialization->setIdentifier(declaration->getIdentifier());
        arrayInitialization->setType(declaration->getType());
        arrayInitialization->setSize(declaration->getSize());


        arrayInitialization->setValues(std::move(values));

        if (m_CurrentToken != token::SEMI) {
            return parseError<ASTArrayInitializationNode>(
                    "File: {}:{}\n\tExpecting a ';' after an array initialization insted of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        return arrayInitialization;
    }

    std::unique_ptr<ASTExprNode> Parser::parseExpr() {
        // Expressions can start with a parenthesis, a dot, a number, a bool lit, an identifier,
        // a squirly bracket (for arrays and structs) or an unary operator.
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

        if (m_CurrentToken == token::TRUE ||
                m_CurrentToken == token::FALSE) {
            auto boolLit = std::make_unique<ASTBoolLiteralExpr>(m_SymbolTable);

            boolLit->setValue(m_CurrentToken == token::TRUE);

            m_CurrentToken = m_Lexer.getNextToken(); // Eat bool lit

            return boolLit;
        }

        if (m_CurrentToken == token::BRA_O) {
            exprTmp = parseArgumentList();
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

        m_CurrentToken = m_Lexer.getNextToken(); // Eat ')'

        auto parExpr = std::make_unique<ASTParExpr>(m_SymbolTable);
        parExpr -> setExpr(std::move(expr));

        return parExpr;
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

    // We want to parse a lot here: for example aStruct.aStructAttibute.aMethod() should return
    // a ASTFunctionCallExpr.
    std::unique_ptr<ASTCallableExpr> Parser::parseIdentifierExpr() {
        std::unique_ptr<ASTCallableExpr> assignableExpr;
        auto identifierExpr = std::make_unique<ASTIdentifierExpr>(m_SymbolTable);
        identifierExpr->setIdentifier(m_CurrentToken.identifier);
        assignableExpr = std::move(identifierExpr);

        m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier
        while (m_CurrentToken == token::PAR_O
                || m_CurrentToken == token::DOT
                || m_CurrentToken == token::ACC_O) {
            switch(m_CurrentToken.token) {
                case token::PAR_O: {
                    auto functionCallExpr = std::make_unique<ASTFunctionCallExpr>(m_SymbolTable);
                    auto argumentList = parseArgumentList(); // Should eat ')'
                    if (auto callable = dynamic_cast<ASTCallableExpr*>(assignableExpr.release())) {
                        auto uniqueCallable = std::make_unique<ASTCallableExpr>(m_SymbolTable);
                        uniqueCallable.reset(callable);
                        functionCallExpr->setFunction(std::move(uniqueCallable));
                        functionCallExpr->setArguments(std::move(argumentList));

                        assignableExpr = std::move(functionCallExpr);
                        break;
                    }
                    // TODO: more verbose.
                    return parseError<ASTAssignableExpr>("Trying to call a non callable expressions");
                }
                case token::DOT: {
                    m_CurrentToken = m_Lexer.getNextToken(); // Eat '.'
                    if (m_CurrentToken == token::IDENT) {
                        auto attributeAccessExpr = std::make_unique<ASTAttributeAccessExpr>(m_SymbolTable);
                        identifierExpr = std::make_unique<ASTIdentifierExpr>(m_SymbolTable);
                        identifierExpr->setIdentifier(m_CurrentToken.identifier);

                        m_CurrentToken = m_Lexer.getNextToken(); // Eat IDENT
                        if (auto accessible = dynamic_cast<ASTAccessibleExpr*>(assignableExpr.release())){
                            auto uniqueAccessible = std::make_unique<ASTAccessibleExpr>(m_SymbolTable);
                            uniqueAccessible.reset(accessible);
                            attributeAccessExpr->setStruct(std::move(uniqueAccessible));
                            attributeAccessExpr->setAttribute(std::move(identifierExpr));

                            assignableExpr = std::move(attributeAccessExpr);
                            break;
                        }
                        return parseError<ASTAssignableExpr>("Trying to access an attribute from a non "
                            "accessible expression");
                    }
                    // TODO: more verbose
                    return parseError<ASTAssignableExpr>("Expecting an identifier after '.'");
                }
                case token::ACC_O: {
                    m_CurrentToken = m_Lexer.getNextToken(); // Eat '['
                    auto indexExpr = parseExpr(); // Current char should be ']'

                    if (m_CurrentToken != token::ACC_C) {
                        return parseError<ASTAccessibleExpr>(
                                "File {}:{}\n\tExpecting a ']' after an array access isntead of {}",
                                m_FilePath,
                                m_CurrentToken.pos,
                                m_CurrentToken
                            );
                    }

                    m_CurrentToken = m_Lexer.getNextToken(); // Eat ']'

                    auto arrayAccessExpr = std::make_unique<ASTArrayAccessExpr>(m_SymbolTable);
                    if (auto accessible = dynamic_cast<ASTAccessibleExpr*>(assignableExpr.release())){
                        auto uniqueAccessible = std::make_unique<ASTAccessibleExpr>(m_SymbolTable);
                        uniqueAccessible.reset(accessible);
                        arrayAccessExpr->setArray(std::move(uniqueAccessible));
                        arrayAccessExpr->setIndex(std::move(indexExpr));

                        assignableExpr = std::move(arrayAccessExpr);
                        break;
                    }

                    return parseError<ASTAssignableExpr>("Trying to access an array index from a"
                            "non accessible expression: {}", typeid(assignableExpr).name());
                }
                default:
                    return parseError<ASTAssignableExpr>("This should never happen at {}: {}",
                            __FILE__,
                            __func__);
            }
        }

        return assignableExpr;
    }

    std::unique_ptr<ASTUnaryExpr> Parser::parseUnaryExpr() {
        token savedToken = m_CurrentToken.token;

        m_CurrentToken = m_Lexer.getNextToken(); // Eat the unary operator

        auto expr = parseExpr();

        switch (savedToken) {
            case token::NOT: {
                auto notExpr = std::make_unique<ASTNotExpr>(m_SymbolTable);
                notExpr->setValue(std::move(expr));
                return notExpr;
            }
            case token::MINUS:{
              auto negExpr = std::make_unique<ASTNegExpr>(m_SymbolTable);
              negExpr->setValue(std::move(expr));
              return negExpr;
            }
            default:
                return parseError<ASTUnaryExpr>("This should never happen at {}: {}",
                        __FILE__,
                        __func__);
        }
    }

    std::unique_ptr<ASTBinaryExpr> Parser::parseBinaryExpr(std::unique_ptr<ASTExprNode> lhs) {
        auto binaryExpr = std::make_unique<ASTBinaryExpr>(m_SymbolTable);
        auto op = ASTNode::TokenToOperator(m_CurrentToken.token);

        m_CurrentToken = m_Lexer.getNextToken(); // Eat operator
        auto rhs = parseExpr();

        binaryExpr->setLHS(std::move(lhs));
        binaryExpr->setRHS(std::move(rhs));
        binaryExpr->setOperator(op);

        if (auto binaryRHS = dynamic_cast<ASTBinaryExpr*>(rhs.release())) {
            if (getOpPrecedence(binaryRHS->getOperator()) > getOpPrecedence(op)) {
                auto newLHS = std::make_unique<ASTBinaryExpr>(m_SymbolTable);
                newLHS->setLHS(std::move(binaryExpr->getLHSPtr()));
                newLHS->setRHS(std::move(binaryRHS->getLHSPtr()));
                newLHS->setOperator(op);

                binaryExpr->setRHS(std::move(newLHS));
                binaryExpr->setOperator(binaryRHS->getOperator());
                binaryExpr->setRHS(std::move(binaryRHS->getRHSPtr()));
            }
        }

        return binaryExpr;
    }

    std::unique_ptr<ASTArgumentList> Parser::parseArgumentList() {
        auto argumentList = std::make_unique<ASTArgumentList>(m_SymbolTable);

        m_CurrentToken = m_Lexer.getNextToken(); // Eat '{' or '('

        while (m_CurrentToken != token::PAR_C &&
                m_CurrentToken != token::BRA_C) {
            auto arg = parseExpr();

            argumentList->addArgument(std::move(arg));

            if (m_CurrentToken != token::PAR_C &&
                    m_CurrentToken != token::BRA_C &&
                    m_CurrentToken != token::COMMA)
                return parseError<ASTArgumentList>("Expecting a ',', a ')' or a '}' in an argument list.");

            if (m_CurrentToken == token::COMMA) {
                m_CurrentToken = m_Lexer.getNextToken(); // Eat ','
            }
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat ')' or '}'

        return argumentList;
    }

} // namespace yapl
