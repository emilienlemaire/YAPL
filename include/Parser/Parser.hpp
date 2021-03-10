/**
 * include/Parser/Parser.hpp
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

#include <CppLogger2/CppLogger2.h>
#include <array>
#include <cstddef>
#include <memory>
#include <stack>
#include <string>
#include <type_traits>
#include <queue>

#include "AST/ASTExprNode.hpp"
#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"
#include "Lexer/Lexer.hpp"
#include "Symbol/SymbolTable.hpp"

namespace yapl {
    class Parser {
    private:
        CppLogger::CppLogger m_Logger;
        Lexer m_Lexer;

        const std::string m_FilePath;

        std::unique_ptr<ASTProgramNode> m_Program;

        Token m_CurrentToken;

        std::shared_ptr<SymbolTable> m_SymbolTable;

        template<typename Ptr, typename... T>
            std::unique_ptr<Ptr> parseError(const std::string &msg, T... var) {
                m_Logger.printError(msg, var...);
                return nullptr;
            }

        void parseInfo(const std::string&);

        template<typename... T>
        void logParser(const std::string &msg, T... var) {
#ifdef LOG_PARSER
            m_Logger.printInfo(msg, var...);
#endif
        }

        static int getOpPrecedence(Operator t_Operator);
    public:
        explicit Parser(const std::string &file="", CppLogger::Level level=CppLogger::Level::Warn);
        std::unique_ptr<ASTNode> parseNext();
        void parse();
        std::unique_ptr<ASTProgramNode> getProgram();

    private:
        std::unique_ptr<ASTNode> parseIdentifier(const std::string&);

        std::unique_ptr<ASTImportNode> parseImport();
        std::unique_ptr<ASTExportNode> parseExport();
        std::unique_ptr<ASTFunctionDefinitionNode> parseFunctionDefinition();
        std::unique_ptr<ASTStructDefinitionNode> parseStructDefinition();
        std::unique_ptr<ASTDeclarationNode> parseDeclaration(const std::string&);
        std::unique_ptr<ASTArrayDeclarationNode> parseArrayDeclaration(std::unique_ptr<ASTDeclarationNode>);
        std::unique_ptr<ASTInitializationNode> parseInitialization(std::unique_ptr<ASTDeclarationNode>);
        std::unique_ptr<ASTArrayInitializationNode> parseArrayInitialization(std::unique_ptr<ASTArrayDeclarationNode>);

        std::unique_ptr<ASTExprNode> parseExpr();
        std::unique_ptr<ASTExprNode> parseParenExpr();
        std::unique_ptr<ASTNumberExpr> parseNumberExpr();
        std::unique_ptr<ASTFloatNumberExpr> parseFloatingNumberExpr(const std::string&);
        std::unique_ptr<ASTIntegerNumberExpr> parseIntegerNumberExpr();
        std::unique_ptr<ASTCallableExpr> parseIdentifierExpr();
        std::unique_ptr<ASTUnaryExpr> parseUnaryExpr();
        std::unique_ptr<ASTBinaryExpr> parseBinaryExpr(std::unique_ptr<ASTExprNode>);

        std::unique_ptr<ASTArgumentList> parseArgumentList();
    };
} // namespace yapl
