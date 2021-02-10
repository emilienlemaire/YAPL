#pragma once

#include <CppLogger2/CppLogger2.h>
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_set>

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

        std::unique_ptr<ASTProgramNode> m_Program;

        Token m_CurrentToken;

        std::shared_ptr<SymbolTable> m_SymbolTable;

        template<typename Ptr, typename... T>
            std::unique_ptr<Ptr> parseError(std::string msg, T... var) {
                m_Logger.printError(msg, var...);
                return nullptr;
            }

        void parseInfo(std::string);

        template<typename... T>
        void logParser(std::string msg, T... var) {
#ifdef LOG_PARSER
            m_Logger.printInfo(msg, var...);
#endif
        }

        int getOpPrecedence(Operator t_Operator);
    public:
        Parser(std::string file="", CppLogger::Level level=CppLogger::Level::Warn);
        std::unique_ptr<ASTNode> parseNext();
        void parse();
        std::unique_ptr<ASTProgramNode> getProgram();

    private:
        std::unique_ptr<ASTImportNode> parseImport();
        std::unique_ptr<ASTExportNode> parseExport();
        std::unique_ptr<ASTFunctionDefinitionNode> parseFunctionDefinition();
        std::unique_ptr<ASTStructDefinitionNode> parseStructDefinition();
        std::unique_ptr<ASTNode> parseIdentifier(const std::string&);

        std::unique_ptr<ASTExprNode> parseExpr();
    };
}
