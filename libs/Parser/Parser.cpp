#include "Parser/Parser.hpp"
#include "AST/ASTExprNode.hpp"
#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "CppLogger2/include/Format.h"
#include "Lexer/TokenUtils.hpp"
#include <cstddef>
#include <memory>
#include <type_traits>
#include <vector>

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

    if (m_CurrentToken == token::bopen) {
        return parseBlock();
    }

    if (m_CurrentToken == token::iflabel) {
        return parseIf();
    }

    if (m_CurrentToken == token::forlabel) {
        return parseFor();
    }

    if (m_CurrentToken == token::func) {
        return parseFunctionDefinition();
    }

    if (m_CurrentToken == token::structlabel) {
        return parseStructDefintion();
    }

    if (m_CurrentToken == token::label) {
        return parseLabel(m_CurrentToken.identifier);
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

    if (m_CurrentToken.token == token::access_sym) {
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
        return parseInitialization(name, declarationType);
    }

    if (m_CurrentToken == token::iopen) {
        return parseArrayDefinition(declarationType, name);
    }

    if (m_CurrentToken != token::semicolon || m_CurrentToken != token::inlabel) {
        return parseError<ASTDeclarationNode>(
                "Syntax error: ';' expected after declaration");
    }

    return std::make_unique<ASTDeclarationNode>(name, declarationType);
}

std::unique_ptr<ASTInitializationNode> Parser::parseInitialization(std::string name, ASTNode::TYPE type) {
    std::unique_ptr<ASTExprNode> expr = parseExpr();

    return std::make_unique<ASTInitializationNode>(name, type, std::move(expr));
}

std::unique_ptr<ASTBlockNode> Parser::parseBlock() {
    std::vector<std::unique_ptr<ASTNode>> nodes;
    while (m_CurrentToken != token::bclose) {
        std::unique_ptr<ASTNode> node = parseNext();
        nodes.push_back(std::move(node));
    }

    return std::make_unique<ASTBlockNode>(std::move(nodes));
}

std::unique_ptr<ASTIfNode> Parser::parseIf() {
    std::unique_ptr<ASTExprNode> condition = parseExpr();

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::bopen) {
        return parseError<ASTIfNode>("Expecting '{' instead of {}", m_CurrentToken);
    }

    std::unique_ptr<ASTBlockNode> ifBlock = parseBlock();

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::elselabel) {
        m_CurrentToken = m_Lexer.getNextToken();
        if (m_CurrentToken != token::bopen) {
            return parseError<ASTIfNode>("Expecting '{' instead of {}", m_CurrentToken);
        }

        std::unique_ptr<ASTBlockNode> elseBlock = parseBlock();

        return std::make_unsigned<ASTIfNode>(std::move(condition), std::move(ifBlock), std::move(elseBlock));
    }

    return std::make_unique<ASTIfNode>(std::move(condition), std::move(ifBlock), nullptr);
}

std::unique_ptr<ASTForNode> Parser::parseFor() {
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::paropen) {
        return parseError<ASTForNode>("Syntax Error: Expecting '(' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::type) {
        return parseError<ASTForNode>("Syntax Error: Expecting a declaration.");
    }
    
    std::unique_ptr<ASTDeclarationNode> iterator = parseDeclaration();

    if (m_CurrentToken != token::inlabel) {
        return parseError<ASTForNode>("SYntax Error: Expecting 'in' instead of {}", m_CurrentToken);
    }

    std::unique_ptr<ASTExprNode> cond = parseRange();

    if (m_CurrentToken != token::parclose) {
        return parseError<ASTForNode>("Syntax Error: Expecting ')' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::bopen) {
        return parseError<ASTForNode>("Syntax Error: Expecting '{' insted of {}", m_CurrentToken);
    }

    std::unique_ptr<ASTBlockNode> block = parseBlock();
    
    return std::make_unique<ASTForNode>(std::move(iterator), std::move(cond), std::move(block));
}

std::unique_ptr<ASTFunctionDefinitionNode> Parser::parseFunctionDefinition() {
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::label) {
        return parseError<ASTFunctionDefinitionNode>("Syntax Error: Expecting a label instead of {}", m_CurrentToken);
    }

    std::string name = m_CurrentToken.identifier;

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::paropen) {
        return parseError<ASTFunctionDefinitionNode>("Syntax Error: Expecting '(' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    std::vector<std::unique_ptr<ASTDeclarationNode>> args;

    while (m_CurrentToken == token::type) {
        ASTNode::TYPE type = ASTNode::stringToType(m_CurrentToken.identifier);
        m_CurrentToken = m_Lexer.getNextToken();
        if (m_CurrentToken != token::label) {
            return parseError<ASTFunctionDefinitionNode>(
                    "Syntax Error: Expecting a label instead of {}",
                    m_CurrentToken
                );
        }
        std::string argName = m_CurrentToken.identifier;

        m_CurrentToken = m_Lexer.getNextToken();

        args.push_back(std::make_unique<ASTDeclarationNode>(argName, type));

        if (m_CurrentToken == token::comma) {
            m_CurrentToken = m_Lexer.getNextToken();
        }
    }

    if (m_CurrentToken != ) {
    statements
    }
}
