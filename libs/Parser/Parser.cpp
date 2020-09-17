#include "Parser/Parser.hpp"
#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "CppLogger2/include/Format.h"
#include "Lexer/TokenUtils.hpp"
#include <cstddef>
#include <memory>
#include <type_traits>

Parser::Parser(std::string filepath, CppLogger::Level level)
    : m_Logger(level, "Parser"), m_Lexer(filepath)
{
    CppLogger::Format format({
            CppLogger::FormatAttribute::Name,
            CppLogger::FormatAttribute::Level,
            CppLogger::FormatAttribute::Message
        });

    m_Logger.setFormat(format);

    parseError<std::nullptr_t>("This is a test");
    parseError<std::nullptr_t>("This is a test with {}", 1);
}

void Parser::parse() {
    while (m_Lexer.peekToken().token != token::eof) {
        parseNext();
    }
}

std::unique_ptr<ASTNode> Parser::parseNext() {
    m_CurrentToken = m_Lexer.getNextToken();
    if (m_CurrentToken.token == token::importlabel) {
        return parseImport();
    }

    if (m_CurrentToken == token::exportlalbel) {
        return parseExport();
    }

    if (m_CurrentToken == token::type) {
        return parseDeclaration();
    }

    return nullptr;
}

std::unique_ptr<ASTImportNode> Parser::parseImport() {
    m_CurrentToken = m_Lexer.getNextToken();
    if (m_CurrentToken.token != token::label) {
        return std::move(parseError<ASTImportNode>(
                    "Syntax error: expected label instead of {}",
                    tokenToString(m_CurrentToken.token)
                    ));
    }

    std::string module = m_CurrentToken.identifier;

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken.token == token::semicolon) {
        return std::make_unique<ASTImportNode>(module);
    }

    if (m_CurrentToken.token == token::colon) {
        m_CurrentToken = m_Lexer.getNextToken();
        if (m_CurrentToken != token::colon) {
            return std::move(parseError<ASTImportNode>(
                        "Syntax error: Expected ':' insted of {}",
                        m_CurrentToken
                        ));
        }

        m_CurrentToken = m_Lexer.getNextToken();

        if (m_CurrentToken != token::bopen) {
            return std::move(parseError<ASTImportNode>(
                        "Syntax error: Expected '{' instead of {}",
                        m_CurrentToken
                        ));
        }

        m_CurrentToken = m_Lexer.getNextToken();

        if (m_CurrentToken != token::label) {
        return std::move(parseError<ASTImportNode>(
                    "Syntax error: Expected a label instead of {}", m_CurrentToken));
        }

        std::vector<std::string> subModules;

        subModules.push_back(m_CurrentToken.identifier);

        m_CurrentToken = m_Lexer.getNextToken();

        while (m_CurrentToken == token::comma) {
            m_CurrentToken = m_Lexer.getNextToken();

            if (m_CurrentToken != token::label) {
                return std::move(parseError<ASTImportNode>(
                            "Syntax error: Expected a label instead of {}",
                            m_CurrentToken
                            ));
            }

            subModules.push_back(m_CurrentToken.identifier);

            m_CurrentToken = m_Lexer.getNextToken();
        }

        if (m_CurrentToken != token::bclose) {
            return std::move(parseError<ASTImportNode>(
                        "Syntax error: Expected '}' instead of {}",
                        m_CurrentToken));
        }

        m_CurrentToken = m_Lexer.getNextToken();

        if (m_CurrentToken != token::semicolon) {
            return std::move(parseError<ASTImportNode>(
                        "Syntax error: Expected ';' instead of {}",
                        m_CurrentToken));
        }

        m_Logger.printInfo(
                "Parsed import statement: module {}, \n \t sub-modules: {}",
                module,
                subModules);

        return std::make_unique<ASTImportNode>(module, subModules);
    }

    return nullptr;
}

std::unique_ptr<ASTExportNode> Parser::parseExport() {
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::structlabel) {
        std::unique_ptr<ASTStructDefinitionNode> exportStruct = parseStructDefintion();
        return std::make_unique<ASTExportNode>(std::move(exportStruct));
    }

    if (m_CurrentToken == token::func) {
        std::unique_ptr<ASTFunctionDefinitionNode> exportFunc = parseFunctionDefinition();
        return std::make_unique<ASTExportNode>(std::move(exportFunc));
    }

    return parseError<ASTExportNode>("Syntax error: 'export' expects a function or struct definition");
}

std::unique_ptr<ASTDeclarationNode> Parser::parseDeclaration() {
    std::string type = m_CurrentToken.identifier;
    
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::label) {
        return parseError<ASTDeclarationNode>(
                "Syntax error: expecting a label instead of {}", m_CurrentToken);
    }

    std::string name = m_CurrentToken.identifier;
    ASTNode::TYPE declarationType = ASTNode::stringToType(type);

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::eq) {
        switch (declarationType) {
            case ASTNode::TYPE::NONE:
                return parseError<ASTDeclarationNode>( "This should never happend");
            case ASTNode::TYPE::INT:
                return parseInitialization<int>(name);
            case ASTNode::TYPE::DOUBLE:
                return parseInitialization<double>(name);
            case ASTNode::TYPE::BOOL:
                return parseInitialization<bool>(name);
            case ASTNode::TYPE::STRING:
                return parseInitialization<std::string>(name);
            case ASTNode::TYPE::VOID:
                return parseError<ASTDeclarationNode>("Syntax error: void type virable cannot be declared");
        }
    }

    if (m_CurrentToken == token::iopen) {
        return parseArrayDefinition(declarationType, name);
    }

    if (m_CurrentToken != token::semicolon) {
        return parseError<ASTDeclarationNode>(
                "Syntax error: ';' expected after declaration");
    }

    return std::make_unique<ASTDeclarationNode>(name, declarationType);
}
