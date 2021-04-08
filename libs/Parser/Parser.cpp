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

    int Parser::getOpPrecedence(Operator t_Operator) {
        switch (t_Operator) {
            case Operator::TIMES:
            case Operator::BY:
            case Operator::MOD:
                return (int)OperatorPrec::MULT;
            case Operator::PLUS:
            case Operator::MINUS:
                return (int)OperatorPrec::ADD;
            case Operator::LTH:
            case Operator::MTH:
            case Operator::LEQ:
            case Operator::MEQ:
                return (int)OperatorPrec::COMP;
            case Operator::EQ:
            case Operator::NEQ:
                return (int)OperatorPrec::EQ;
            case Operator::AND:
                return (int)OperatorPrec::AND;
            case Operator::OR:
                return (int)OperatorPrec::OR;
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
            auto identNode = parseIdentifier(m_CurrentToken.identifier);
            if (m_CurrentToken != token::SEMI){
                return parseError<ASTDeclarationNode>(
                        "File: {}:{}\n\tMissing a ';' after a statement instead of {}",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

            m_CurrentToken = m_Lexer.getNextToken(); // Eat ';'

            return identNode;
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

            if (m_CurrentToken == token::BRA_O) { // Cas namespace::{value, value1};
                break;
            }

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
        m_CurrentToken = m_Lexer.getNextToken(); // Eat 'func'

        auto functionDefinition = std::make_unique<ASTFunctionDefinitionNode>(m_SymbolTable);

        if (m_CurrentToken != token::IDENT) {
            return parseError<ASTFunctionDefinitionNode>(
                    "File: {}:{}\n\tExpecting an identifier after 'func' instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        auto funcName = m_CurrentToken.identifier;

        m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier

        if (m_CurrentToken != token::PAR_O) {
            return parseError<ASTFunctionDefinitionNode>(
                    "File: {}:{}\n\tExpecting a parameter list after a function identifier instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat '('

        // This should allow for default parameters, let's try
        bool hasDefaultParameter = false;

        while (m_CurrentToken != token::PAR_C) {
            if (m_CurrentToken != token::IDENT) {
                return parseError<ASTFunctionDefinitionNode>(
                        "File: {}:{}\n\tExpecting a declaration in function parameters list instead of {}",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

            auto typeName = m_CurrentToken.identifier;

            m_CurrentToken = m_Lexer.getNextToken(); // Eat type identifier

            auto declaration = parseDeclaration(typeName);

            bool isInitialization = false;
            if (dynamic_cast<ASTInitializationNode*>(declaration.get())) {
                isInitialization = true;
            }

            if (dynamic_cast<ASTArrayInitializationNode*>(declaration.get())) {
                isInitialization = true;
            }

            hasDefaultParameter = hasDefaultParameter || isInitialization;

            if (hasDefaultParameter && !isInitialization) {
                return parseError<ASTFunctionDefinitionNode>(
                        "File: {}:{}\n\tIn {} declaration, all subsequent parameters to a default "
                        "parameter must be defaulted too.",
                        m_FilePath,
                        m_CurrentToken.pos,
                        funcName
                    );
            }

            functionDefinition->addParameter(std::move(declaration));

            if (m_CurrentToken == token::COMMA) {
                m_CurrentToken = m_Lexer.getNextToken();

                if (m_CurrentToken != token::IDENT) {
                    return parseError<ASTFunctionDefinitionNode>(
                            "File: {}:{}\n\tExpecting a type identifier after a ',' in parameters list "
                            "instead of {}",
                            m_FilePath,
                            m_CurrentToken.pos,
                            m_CurrentToken
                        );
                }
            } else if (m_CurrentToken != token::PAR_C) {
                return parseError<ASTFunctionDefinitionNode>(
                        "File; {}:{}\n\tExpecting a ')' at the end of parameters list instead of {}",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat ')'

        if (m_CurrentToken != token::ARROW) {
            return parseError<ASTFunctionDefinitionNode>(
                    "File: {}:{}\n\tExpecting an '->' after function parameters list isntead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat '->'

        if (m_CurrentToken != token::IDENT) {
            return parseError<ASTFunctionDefinitionNode>(
                    "File: {}:{}\n\tExpecting a type identifier after '->' instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        auto typeName = m_CurrentToken.identifier;

        m_CurrentToken = m_Lexer.getNextToken(); // Eat type identifier

        if (m_CurrentToken != token::BRA_O) {
            return parseError<ASTFunctionDefinitionNode>(
                    "File: {}:{}\n\tExpecting a '{' at start of function body instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_SymbolTable = m_SymbolTable->pushScope(m_SymbolTable); // We enter the body scope

        std::vector<std::shared_ptr<Value>> parametersValue = {};

        // TODO: Create overload for each default parameters
        for (const auto &param : functionDefinition->getParameters()) {
            auto paramType = m_SymbolTable->lookup(param->getType());
            auto paramValue = Value::CreateVariableValue(param->getIdentifier(), paramType);
            parametersValue.push_back(paramValue);
            m_SymbolTable->insert(paramValue);
        }

        m_CurrentToken = m_Lexer.getNextToken();

        auto body = parseBlock(); // Eats '}'

        functionDefinition->setFunctionName(funcName);
        functionDefinition->setReturnType(typeName);
        functionDefinition->setBody(std::move(body));

        m_SymbolTable = m_SymbolTable->popScope(); // Exit the body scope

        auto typeValue = m_SymbolTable->lookup(typeName);
        auto functionValue = Value::CreateFunctionValue(funcName, typeValue, parametersValue);

        // TODO: Allow function overload
        if (!m_SymbolTable->insert(functionValue)) {
            return parseError<ASTFunctionDefinitionNode>(
                    "File: {}:{}\n\tRedfinition of function {}.",
                    m_FilePath,
                    m_CurrentToken.pos,
                    funcName
                );
        }

        return functionDefinition;
    }

    std::unique_ptr<ASTStructDefinitionNode> Parser::parseStructDefinition() {
        m_CurrentToken = m_Lexer.getNextToken(); // Eat 'struct'

        if (m_CurrentToken != token::IDENT) {
            return parseError<ASTStructDefinitionNode>(
                    "File: {}:{}\n\tExpecting an identifier after 'struct' instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        auto structDef = std::make_unique<ASTStructDefinitionNode>(m_SymbolTable);

        m_SymbolTable = m_SymbolTable->pushScope(m_SymbolTable); // We enter the body scope

        structDef->setStructName(m_CurrentToken.identifier);

        m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier

        // TODO: Enable inheritance

        if (m_CurrentToken != token::BRA_O) {
            return parseError<ASTStructDefinitionNode>(
                    "File: {}:{}\n\tExpecting a '{' after struct name instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat '{'

        while (m_CurrentToken == token::IDENT || m_CurrentToken == token::FUNC) {
            if (m_CurrentToken == token::IDENT) {
                auto type = m_CurrentToken.identifier;

                m_CurrentToken = m_Lexer.getNextToken(); // Eat type

                if (m_CurrentToken != token::IDENT) {
                    return parseError<ASTStructDefinitionNode>(
                            "File: {}:{}\n\tExpecting an identifier after the type in struct definition"
                            " instead of {}",
                            m_FilePath,
                            m_CurrentToken.pos,
                            m_CurrentToken
                        );
                }

                auto declStmt = parseDeclaration(type);

                structDef->addAttribute(std::move(declStmt));

                if (m_CurrentToken != token::SEMI) {
                    return parseError<ASTStructDefinitionNode>(
                            "File: {}:{}\n\tExpecting a ';' after attribute declaration instead of {}",
                            m_FilePath,
                            m_CurrentToken.pos,
                            m_CurrentToken
                        );
                }

                m_CurrentToken = m_Lexer.getNextToken(); // Eat ';'
            }

            if (m_CurrentToken == token::FUNC) {
                auto funcDef = parseFunctionDefinition();
                structDef->addMethod(std::move(funcDef));
            }
        }

        if (m_CurrentToken != token::BRA_C) {
            return parseError<ASTStructDefinitionNode>(
                    "File: {}:{}\n\tExpecting a '}' after a struct defintion instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        // TODO: Add the constructor, (maybe in semantic analysis)

        m_SymbolTable = m_SymbolTable->popScope();

        std::vector<std::shared_ptr<Type>> attrType;

        for (const auto &attr : structDef->getAttributes()) {
            auto typeName = attr->getType();
            auto typeValue = m_SymbolTable->lookup(typeName);

            attrType.push_back(typeValue->getType());
        }

        auto structType = Type::CreateStructType(structDef->getStructName(), attrType);

        auto structTypeValue = Value::CreateTypeValue(structDef->getStructName(), structType);

        m_SymbolTable->insert(std::move(structTypeValue));

        m_CurrentToken = m_Lexer.getNextToken(); // Eat '}'

        return structDef;
    }

    std::unique_ptr<ASTNode> Parser::parseIdentifier(const std::string &identifier) {
        const std::string& firstIdentifier = std::string(identifier);
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

        if (m_CurrentToken == token::PAR_O) {
            return parseStructConstructorInitialization(std::move(declarationNode));
        }

        return declarationNode;
    }

    std::unique_ptr<ASTInitializationNode> Parser::parseInitialization(
            std::unique_ptr<ASTDeclarationNode> declaration
        ) {
        auto type = declaration->getType();
        auto identifier = declaration->getIdentifier();

        auto initialization = std::make_unique<ASTInitializationNode>(m_SymbolTable);

        if (auto expr = parseExpr()) {
            initialization->setIdentifier(identifier);
            initialization->setType(type);
            initialization->setValue(std::move(expr));

            auto typeValue = m_SymbolTable->lookup(type);
            auto initValue = Value::CreateVariableValue(identifier, typeValue);

            m_SymbolTable->insert(initValue);

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

        m_CurrentToken = m_Lexer.getNextToken(); // Eat ']'

        if (m_CurrentToken == token::ACC_O) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat '['

            return parseArrayDeclaration(std::move(arrayDeclaration));
        }

        if (m_CurrentToken == token::ASSIGN) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat '='

            return parseArrayInitialization(std::move(arrayDeclaration));
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
                    "File: {}:{}\n\tExpecting an expression",
                    m_FilePath,
                    m_CurrentToken.pos
                );
        }

        auto arrayInitialization = std::make_unique<ASTArrayInitializationNode>(m_SymbolTable);
        arrayInitialization->setIdentifier(declaration->getIdentifier());
        arrayInitialization->setType(declaration->getType());
        arrayInitialization->setSize(declaration->getSize());


        arrayInitialization->setValues(std::move(values));

        return arrayInitialization;
    }

    std::unique_ptr<ASTStructInitializationNode> Parser::parseStructConstructorInitialization(
            std::unique_ptr<ASTDeclarationNode> declaration
        ) {
        auto structInit = std::make_unique<ASTStructInitializationNode>(m_SymbolTable);
        structInit->setType(declaration->getType());
        structInit->setIdentifier(declaration->getIdentifier());

        auto arguments = parseArgumentList(); // Eats ')'

        structInit->setAttributeValues(std::move(arguments));

        return structInit;
    }

    std::unique_ptr<ASTBlockNode> Parser::parseBlock() {
        auto block = std::make_unique<ASTBlockNode>(m_SymbolTable);

        while (m_CurrentToken != token::BRA_C) {
            if (m_CurrentToken == token::IF) {
                auto ifNode = parseIf();

                if(!ifNode) {
                    return parseError<ASTBlockNode>("If wrong!!!");
                }

                block->addStatement(std::move(ifNode));
                continue;
            }

            if (m_CurrentToken == token::FOR) {
                auto forNode = parseFor();

                if(!forNode) {
                    return parseError<ASTBlockNode>("For wrong!!!");
                }

                block->addStatement(std::move(forNode));
                continue;
            }

            if (m_CurrentToken == token::RETURN) {
                auto returnNode = parseReturn();

                if(!returnNode) {
                    return parseError<ASTBlockNode>("Return wrong!!!");
                }

                if (m_CurrentToken != token::SEMI) {
                    return parseError<ASTBlockNode>(
                            "File: {}:{}\n\tExpecting a ';' after a return statement instead of {}",
                            m_FilePath,
                            m_CurrentToken.pos,
                            m_CurrentToken
                        );
                }

                m_CurrentToken = m_Lexer.getNextToken(); // Eat ';'

                block->addStatement(std::move(returnNode));
                continue;
            }

            if (m_CurrentToken == token::IDENT) {
                auto oldToken = m_CurrentToken;
                auto firstIdentifier = m_CurrentToken.identifier;

                m_CurrentToken = m_Lexer.getNextToken(); // Eat first identifier

                if (m_CurrentToken == token::IDENT) {
                    auto declStmt = parseDeclaration(firstIdentifier);

                    if (m_CurrentToken != token::SEMI) {
                        return parseError<ASTBlockNode>(
                                "File: {}:{}\n\tExpecting a ';' after a declaration statemennt instead of {}",
                                m_FilePath,
                                m_CurrentToken.pos,
                                m_CurrentToken
                            );
                    }

                    m_CurrentToken = m_Lexer.getNextToken(); // Eat ';'

                    block->addStatement(std::move(declStmt));
                    continue;
                }

                auto exprTmp = parseExpr(oldToken);

                if (m_CurrentToken == token::ASSIGN) {
                    auto assignNode = parseAssignment(std::move(exprTmp));

                    if (m_CurrentToken != token::SEMI) {
                        return parseError<ASTBlockNode>(
                                "File: {}:{}\n\tExpecting a ';' after an assignement instead of {}",
                                m_FilePath,
                                m_CurrentToken.pos,
                                m_CurrentToken
                                );
                    }

                    m_CurrentToken = m_Lexer.getNextToken(); // Eat ';'

                    block->addStatement(std::move(assignNode));
                    continue;
                }

                auto exprStmt = std::make_unique<ASTExprStatementNode>(m_SymbolTable);

                exprStmt->setExpr(std::move(exprTmp));

                if (m_CurrentToken != token::SEMI) {
                    return parseError<ASTBlockNode>(
                            "File: {}:{}\n\tExpecting a ';' after an identifier expression statement instead of {}",
                            m_FilePath,
                            m_CurrentToken.pos,
                            m_CurrentToken
                        );
                }

                m_CurrentToken = m_Lexer.getNextToken(); // Eat ';'

                block->addStatement(std::move(exprStmt));
                continue;
            }

            if (auto expr = parseExpr()) {
                auto exprStmt = std::make_unique<ASTExprStatementNode>(m_SymbolTable);

                exprStmt->setExpr(std::move(expr));

                if (m_CurrentToken != token::SEMI) {
                    return parseError<ASTBlockNode>(
                            "File: {}:{}\n\tExpecting a ';' after an expression statement instead of {}",
                            m_FilePath,
                            m_CurrentToken.pos,
                            m_CurrentToken
                        );
                }

                block->addStatement(std::move(exprStmt));
                continue;
            }

            return parseError<ASTBlockNode>(
                    "File: {}:{}\n\tUnexpected token inside a block: {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat '}'
        return block;
    }

    std::unique_ptr<ASTReturnNode> Parser::parseReturn() {
        m_CurrentToken = m_Lexer.getNextToken(); // Eat 'return'

        auto returnNode = std::make_unique<ASTReturnNode>(m_SymbolTable);

        if (auto expr = parseExpr()) {
            returnNode->setExpr(std::move(expr));

            return returnNode;
        }

        return parseError<ASTReturnNode>(
                "File: {}:{}\n\tFailed to parse the expression after 'return'",
                m_FilePath,
                m_CurrentToken.pos
            );
    }

    std::unique_ptr<ASTIfNode> Parser::parseIf() {
        m_CurrentToken = m_Lexer.getNextToken(); // Eat 'if'

        if (m_CurrentToken != token::PAR_O) {
            return parseError<ASTIfNode>(
                    "File: {}:{}\n\tExpecting a '(' after 'if' instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }


        auto ifStmt = std::make_unique<ASTIfNode>(m_SymbolTable);

        m_CurrentToken = m_Lexer.getNextToken(); // Eat '('

        auto conditionExpr = parseExpr();

        if (!conditionExpr) {
            return parseError<ASTIfNode>(
                    "File: {}:{}\n\tExpecting an expression inside the 'if' condition",
                    m_FilePath,
                    m_CurrentToken.pos
                );
        }

        ifStmt->setCondition(std::move(conditionExpr));

        if (m_CurrentToken != token::PAR_C) {
            return parseError<ASTIfNode>(
                    "File: {}:{}\n\tExpecting a matching ')' after the if condition instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat ')'

        if (m_CurrentToken != token::BRA_O) {
            return parseError<ASTIfNode>(
                    "File: {}:{}\n\tExpecting a '{' after the if condition instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat '{'

        m_SymbolTable = m_SymbolTable->pushScope(m_SymbolTable); // Enter the then block scope

        auto thenBlock = parseBlock(); // Eats '}' of then block

        ifStmt->setThenBlock(std::move(thenBlock));

        m_SymbolTable = m_SymbolTable->popScope(); // Exit then block scope

        if (m_CurrentToken == token::ELSE) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat 'else'

            if (m_CurrentToken != token::BRA_O) {
                return parseError<ASTIfNode>(
                        "File: {}:{}\n\tExpecting a '{' after 'else' instead of {}",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

            m_CurrentToken = m_Lexer.getNextToken(); // Eat '{'

            m_SymbolTable = m_SymbolTable->pushScope(m_SymbolTable); // Enter the else block scope

            auto elseBlock = parseBlock(); // Eats '}' of the else block

            m_SymbolTable = m_SymbolTable->popScope(); // Exit else block scope

            ifStmt->setElseBlock(std::move(elseBlock));
        }

        return ifStmt;
    }

    std::unique_ptr<ASTForNode> Parser::parseFor() {
        m_CurrentToken = m_Lexer.getNextToken(); // Eat 'for'

        if (m_CurrentToken != token::PAR_O) {
            return parseError<ASTForNode>(
                    "File: {}:{}\n\tExpecting a '(' after a 'for' instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_SymbolTable = m_SymbolTable->pushScope(m_SymbolTable); // Enter the for scope

        auto forStmt = std::make_unique<ASTForNode>(m_SymbolTable);

        m_CurrentToken = m_Lexer.getNextToken(); // Eat '('

        if (m_CurrentToken != token::IDENT) {
            return parseError<ASTForNode>(
                    "File: {}:{}\n\tExpecting a type identifier in the for declaration instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        auto type = m_CurrentToken.identifier;

        m_CurrentToken = m_Lexer.getNextToken(); // Eat type

        if (m_CurrentToken != token::IDENT) {
            return parseError<ASTForNode>(
                    "File: {}:{}\n\tExpecting a variable identifier after for declaration type isntead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        auto declStmt = parseDeclaration(type);

        forStmt->setIteratorVariable(declStmt->getIdentifier());

        if (m_CurrentToken != token::IN) {
            return parseError<ASTForNode>(
                    "File: {}:{}\n\tExpecting 'in' after the for iterator variable declaration instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat 'in'

        auto rangeExpr = parseRangeExpr();

        forStmt->setRangeExpr(std::move(rangeExpr));

        if (m_CurrentToken != token::PAR_C) {
            return parseError<ASTForNode>(
                    "File: {}:{}\n\tExpecting a matching ')' instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat ')'

        if (m_CurrentToken != token::BRA_O) {
            return parseError<ASTForNode>(
                    "File: {}:{}\n\tExpecting a '{' after for condition instead of {}",
                    m_FilePath,
                    m_CurrentToken.pos,
                    m_CurrentToken
                );
        }

        m_CurrentToken = m_Lexer.getNextToken(); //Eat '{'

        auto forBlock = parseBlock(); // Eats the '}' at the end of the for block

        forStmt->setBlock(std::move(forBlock));

        m_SymbolTable = m_SymbolTable->popScope(); // Exit the for scope

        return forStmt;
    }

    std::unique_ptr<ASTAssignmentNode> Parser::parseAssignment(std::unique_ptr<ASTExprNode> var) {
        if (auto assignable = static_unique_pointer_cast<ASTAssignableExpr>(std::move(var))) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat '='

            if (auto expr = parseExpr()) {
                auto assignment = std::make_unique<ASTAssignmentNode>(m_SymbolTable);

                assignment->setVariable(std::move(assignable));
                assignment->setValue(std::move(expr));

                return assignment;
            }

            return parseError<ASTAssignmentNode>(
                    "File: {}:{}\n\tInvalid expression after assignment.",
                    m_FilePath,
                    m_CurrentToken.pos
                );
        }

        return parseError<ASTAssignmentNode>(
                "File: {}:{}\n\tNear {}: Trying to assign a value to a non-assignable expression.",
                m_FilePath,
                m_CurrentToken.pos,
                m_CurrentToken);
    }

    /***************
     *             *
     * Expressions *
     *             *
     ***************/
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

            while(m_CurrentToken == token::DOT ||
                    m_CurrentToken == token::PAR_O ||
                    m_CurrentToken == token::ACC_O) {
                switch (m_CurrentToken.token) {
                    case token::DOT:
                        exprTmp = parseAttributeAccess(std::move(exprTmp));
                        break;
                    case token::PAR_O:
                        exprTmp = parseFunctionCall(std::move(exprTmp));
                        break;
                    case token::ACC_O:
                        exprTmp = parseArrayAccess(std::move(exprTmp));
                        break;
                    default:
                        assert(false && "Something went very wrong");
                        break;
                }
            }
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

    std::unique_ptr<ASTExprNode> Parser::parseExpr(Token oldToken) {
        // Expressions can start with a parenthesis, a dot, a number, a bool lit, an identifier,
        // a squirly bracket (for arrays and structs) or an unary operator.
        // We first parse an expression and then if the next token is a binary operator we parse it
        // as such.

        std::unique_ptr<ASTExprNode> exprTmp;

        if (oldToken == token::IDENT) {
            exprTmp = parseIdentifierExpr(oldToken.identifier);

            while(m_CurrentToken == token::DOT ||
                    m_CurrentToken == token::PAR_O ||
                    m_CurrentToken == token::ACC_O) {
                switch (m_CurrentToken.token) {
                    case token::DOT:
                        exprTmp = parseAttributeAccess(std::move(exprTmp));
                        break;
                    case token::PAR_O:
                        exprTmp = parseFunctionCall(std::move(exprTmp));
                        break;
                    case token::ACC_O:
                        exprTmp = parseArrayAccess(std::move(exprTmp));
                        break;
                    default:
                        assert(false && "Something went very wrong");
                        break;
                }
            }
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

        assert(false && "Something went very wrong, this shouldn't happen"); // NOLINT

        return parseError<ASTNumberExpr>("Something went very wrong, this shouldn't happen");
    }

    std::unique_ptr<ASTCallableExpr> Parser::parseIdentifierExpr() {
        auto identifierExpr = std::make_unique<ASTIdentifierExpr>(m_SymbolTable);
        identifierExpr->setIdentifier(m_CurrentToken.identifier);

        m_CurrentToken = m_Lexer.getNextToken(); // Eat identifier
        return identifierExpr;
    }

    std::unique_ptr<ASTCallableExpr> Parser::parseIdentifierExpr(std::string identifier) {
        auto identifierExpr = std::make_unique<ASTIdentifierExpr>(m_SymbolTable);
        identifierExpr->setIdentifier(identifier);

        return identifierExpr;
    }

    std::unique_ptr<ASTAttributeAccessExpr> Parser::parseAttributeAccess(std::unique_ptr<ASTExprNode> expr) {
        if (auto accessible = static_unique_pointer_cast<ASTAccessibleExpr>(std::move(expr))) {
            auto attributeAccess = std::make_unique<ASTAttributeAccessExpr>(m_SymbolTable);

            attributeAccess->setStruct(std::move(accessible));

            m_CurrentToken = m_Lexer.getNextToken(); // Eat '.'

            if (m_CurrentToken != token::IDENT) {
                return parseError<ASTAttributeAccessExpr>(
                        "File: {}:{}\n\tExpecting an identifier after '.' instead of {}.",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

            auto attribute = parseIdentifierExpr(); // Eats last identifier

            attributeAccess->setAttribute(static_unique_pointer_cast<ASTIdentifierExpr>(std::move(attribute)));

            return attributeAccess;
        }

        return parseError<ASTAttributeAccessExpr>(
                "File {}:{}\n\tNear: {}, trying to access a non-accessible expression.",
                m_FilePath,
                m_CurrentToken.pos,
                m_CurrentToken
            );
    }

    std::unique_ptr<ASTArrayAccessExpr> Parser::parseArrayAccess(std::unique_ptr<ASTExprNode> expr) {
        if (auto accessible = static_unique_pointer_cast<ASTAccessibleExpr>(std::move(expr))) {
            auto arrayAccess = std::make_unique<ASTArrayAccessExpr>(m_SymbolTable);

            arrayAccess->setArray(std::move(accessible));

            m_CurrentToken = m_Lexer.getNextToken(); // Eat '['

            auto index = parseExpr();

            if(!index) {
                return parseError<ASTArrayAccessExpr>(
                        "File: {}:{}\n\tNot viable expression for array index.",
                        m_FilePath,
                        m_CurrentToken.pos
                    );
            }

            arrayAccess->setIndex(std::move(index));

            if (m_CurrentToken != token::ACC_C) {
                return parseError<ASTArrayAccessExpr>(
                        "File: {}:{}\n\tExpecting a ']' after array access instead of {}.",
                        m_FilePath,
                        m_CurrentToken.pos,
                        m_CurrentToken
                    );
            }

            m_CurrentToken = m_Lexer.getNextToken(); // Eat ']'

            return arrayAccess;
        }

        return parseError<ASTArrayAccessExpr>(
                "File {}:{}\n\tNear: {}, trying to access a non-accessible expression.",
                m_FilePath,
                m_CurrentToken.pos,
                m_CurrentToken
            );
    }

    std::unique_ptr<ASTFunctionCallExpr> Parser::parseFunctionCall(std::unique_ptr<ASTExprNode> expr) {
        if (auto callable = static_unique_pointer_cast<ASTCallableExpr>(std::move(expr))) {
            auto callExpr = std::make_unique<ASTFunctionCallExpr>(m_SymbolTable);

            callExpr->setFunction(std::move(callable));

            auto arguments = parseArgumentList(); // Eats ')'

            if(!arguments) {
                return parseError<ASTFunctionCallExpr>(
                        "File: {}:{}\n\tNot viable argument list.",
                        m_FilePath,
                        m_CurrentToken.pos
                    );
            }

            callExpr->setArguments(std::move(arguments));

            return callExpr;
        }

        return parseError<ASTFunctionCallExpr>(
                "File {}:{}\n\tNear: {}, trying to access a non-accessible expression.",
                m_FilePath,
                m_CurrentToken.pos,
                m_CurrentToken
            );
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
                    m_CurrentToken != token::COMMA) {
                return parseError<ASTArgumentList>(
                            "File: {}:{}\n\tExpecting a ',', a ')' or a '}' in an argument list instead of {}",
                            m_FilePath,
                            m_CurrentToken.pos,
                            m_CurrentToken
                        );
            }

            if (m_CurrentToken == token::COMMA) {
                m_CurrentToken = m_Lexer.getNextToken(); // Eat ','
            }
        }

        m_CurrentToken = m_Lexer.getNextToken(); // Eat ')' or '}'

        return argumentList;
    }

    std::unique_ptr<ASTRangeExpr> Parser::parseRangeExpr() {
        auto rangeExpr = std::make_unique<ASTRangeExpr>(m_SymbolTable);

        auto startExpr = parseExpr();

        rangeExpr->setStart(std::move(startExpr));

        // If there is no '...' it means the first expression should be iterable (to check in SemA)
        // so we don't need to parse the end expression, nullptr is acceptable here.

        if (m_CurrentToken == token::FROM_TO) {
            m_CurrentToken = m_Lexer.getNextToken(); // Eat '...'

            auto endExpr = parseExpr();

            rangeExpr->setEnd(std::move(endExpr));
        }

        return rangeExpr;
    }

} // namespace yapl
