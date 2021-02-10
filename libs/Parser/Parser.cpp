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
    Parser::Parser(std::string filepath, CppLogger::Level level)
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

    void Parser::parseInfo(std::string info) {
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

        return parseError<ASTNode>(
                "File: {}:{}\n\tUnexpected token at top level scope: {}",
                m_FilePath,
                m_CurrentToken.pos,
                m_CurrentToken);
    }

    /* TODO:
     *  - Make it possible to import several values from on namespace
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
        return nullptr;
    }

    std::unique_ptr<ASTExprNode> Parser::parseExpr() {
        return nullptr;
    }

}
