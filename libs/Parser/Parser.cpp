#include "Parser/Parser.hpp"
#include "AST/ASTExprNode.hpp"
#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "CppLogger2/include/Format.h"
#include "Lexer/TokenUtils.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
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
}

void Parser::parseInfo(std::string info) {
#ifdef LOG_PARSER
    m_Logger.printInfo("Parsing {}", info);
#endif
}

void Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> nodes;
    while (m_Lexer.peekToken().token != token::eof) {
        auto node = parseNext();
        nodes.push_back(std::move(node));
    }

    m_Program = std::make_unique<ASTProgramNode>(std::move(nodes));
}

std::unique_ptr<ASTProgramNode> Parser::getProgram() {
    return std::move(m_Program);
}

std::unique_ptr<ASTNode> Parser::parseNext() {
    parseInfo("next");
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::semicolon) {
        m_CurrentToken = m_Lexer.getNextToken();
    }

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

    if (m_CurrentToken == token::returnlabel) {
        return parseReturn();
    }

    if (m_CurrentToken == token::label) {
        return parseLabel(m_CurrentToken.identifier);
    }

    return parseExpr();
}

std::unique_ptr<ASTNode> Parser::parseNextBlock() {
    parseInfo("next");

    if (m_CurrentToken == token::semicolon) {
        m_CurrentToken = m_Lexer.getNextToken();
    }

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

    if (m_CurrentToken == token::returnlabel) {
        return parseReturn();
    }

    if (m_CurrentToken == token::label) {
        return parseLabel(m_CurrentToken.identifier);
    }

    return parseExpr();
}
std::unique_ptr<ASTExprNode> Parser::parseExpr() {
    std::unique_ptr<ASTExprNode> tmpExpr;

    parseInfo("expr");

    if (m_CurrentToken == token::int_value) {
        parseInfo("int literal");
        tmpExpr = parseLiteral<int>();
    }

    if (m_CurrentToken == token::float_value) {
        parseInfo("double literal");
        tmpExpr = parseLiteral<double>();
    }

    if (m_CurrentToken == token::truelabel || m_CurrentToken == token::falselabel) {
        parseInfo("bool literal");
        tmpExpr = parseLiteral<bool>();
    }

    if (m_CurrentToken == token::dquote) {
        parseInfo("string literal");
        tmpExpr = parseLiteral<std::string>();
    }

    if (m_CurrentToken == token::paropen) {
        return parseParenExpr();
    }


    if (m_CurrentToken == token::label) {
        // Handles Warn: has array member assignment until better parsing
        // Identifier, NamespaceIdentifier, FunctionCall, MethodCall
        auto expr = parseLabelExpr();
    } else {
        m_CurrentToken = m_Lexer.getNextToken();
    }

    if (m_CurrentToken == token::plus
            || m_CurrentToken == token::minus
            || m_CurrentToken == token::times
            || m_CurrentToken == token::divide
            || m_CurrentToken == token::mod
            || m_CurrentToken == token::lth
            || m_CurrentToken == token::mth
            || m_CurrentToken == token::orsym
            || m_CurrentToken == token::andsym
            || m_CurrentToken == token::eqcomp
            || m_CurrentToken == token::leq
            || m_CurrentToken == token::meq) {
        return parseBinary(std::move(tmpExpr));
    }

    if (m_CurrentToken == token::fromto
            || m_CurrentToken == token::fromtol
            || m_CurrentToken == token::fromtominus
            || m_CurrentToken == token::fromoreto) {
        return parseRange(std::move(tmpExpr));
    }

    return std::move(tmpExpr);
}

std::unique_ptr<ASTImportNode> Parser::parseImport() {
    parseInfo("import");
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

        return std::make_unique<ASTImportNode>(module, subModules);
    }

    return parseError<ASTImportNode>("Syntax Error: Expecting '::' or ';' instead of {}", m_CurrentToken);
}

std::unique_ptr<ASTExportNode> Parser::parseExport() {
    parseInfo("export");

    m_CurrentToken = m_Lexer.getNextToken(); // Eat 'export'

    if (m_CurrentToken == token::structlabel) {
        std::unique_ptr<ASTStructDefinitionNode> exportStruct = parseStructDefintion();
        logParser("Parsed struct export");
        return std::make_unique<ASTExportNode>(std::move(exportStruct));
    }

    if (m_CurrentToken == token::func) {
        std::unique_ptr<ASTFunctionDefinitionNode> exportFunc = parseFunctionDefinition();
        logParser("Parsed func export");
        return std::make_unique<ASTExportNode>(std::move(exportFunc));
    }

    return parseError<ASTExportNode>("Syntax error: 'export' expects a function or struct definition");
}

std::unique_ptr<ASTDeclarationNode> Parser::parseDeclaration() {
    parseInfo("declaration");
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

    if (m_CurrentToken != token::semicolon && m_CurrentToken != token::inlabel) {
        return parseError<ASTDeclarationNode>(
                "Syntax error: Expecting ';' or 'in' after declaration instead of {}", m_CurrentToken);
    }

    return std::make_unique<ASTDeclarationNode>(name, declarationType);
}

std::unique_ptr<ASTInitializationNode> Parser::parseInitialization(std::string name, ASTNode::TYPE type) {
    parseInfo("initialization");
    m_CurrentToken = m_Lexer.getNextToken();
    std::unique_ptr<ASTExprNode> expr = parseExpr();

    if (m_CurrentToken != token::semicolon) {
        return parseError<ASTInitializationNode>("Syntax Error: Expected ';' instead of {}", m_CurrentToken);
    }

    return std::make_unique<ASTInitializationNode>(name, type, std::move(expr));
}

std::unique_ptr<ASTReturnNode> Parser::parseReturn() {
    parseInfo("return");
    m_CurrentToken = m_Lexer.getNextToken();

    auto expr = parseExpr();

    if (m_CurrentToken != token::semicolon) {
        return parseError<ASTReturnNode>("Syntax Error: Expecting ';' instead of {}", m_CurrentToken);
    }

    return std::make_unique<ASTReturnNode>(std::move(expr));
}

std::unique_ptr<ASTBlockNode> Parser::parseBlock() {
    parseInfo("block");
    std::vector<std::unique_ptr<ASTNode>> nodes;

    m_CurrentToken = m_Lexer.getNextToken(); // Eat '{'
    while (m_CurrentToken != token::bclose) {
        std::unique_ptr<ASTNode> node = parseNextBlock();
        nodes.push_back(std::move(node));

        if(m_CurrentToken == token::semicolon) {
            m_CurrentToken = m_Lexer.getNextToken();
        }
    }

    return std::make_unique<ASTBlockNode>(std::move(nodes));
}

std::unique_ptr<ASTIfNode> Parser::parseIf() {
    parseInfo("if");
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

        return std::make_unique<ASTIfNode>(std::move(condition), std::move(ifBlock), std::move(elseBlock));
    }

    return std::make_unique<ASTIfNode>(std::move(condition), std::move(ifBlock), nullptr);
}

std::unique_ptr<ASTForNode> Parser::parseFor() {
    parseInfo("for");
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

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::int_value && m_CurrentToken != token::float_value) {
        return parseError<ASTForNode>("Syntax Error: Expecting a literal number instead of {}", m_CurrentToken);
    }

    std::unique_ptr<ASTExprNode> expr;

    if (m_CurrentToken == token::int_value) {
        expr = parseLiteral<int>();
    }

    if (m_CurrentToken == token::float_value) {
        expr = parseLiteral<double>();
    }

    m_CurrentToken = m_Lexer.getNextToken(); // Eat the literal value

    std::unique_ptr<ASTExprNode> cond = parseRange(std::move(expr));

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
    parseInfo("function definition");
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

    if (m_CurrentToken != token::parclose) {
        return parseError<ASTFunctionDefinitionNode>("Syntax Error: Expecting ')' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::arrow_op) {
        return parseError<ASTFunctionDefinitionNode>("SyntaxError: Expecting '->' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::type) {
        return parseError<ASTFunctionDefinitionNode>("Sybtax Error: Expecting a type instead of {}", m_CurrentToken);
    }

    ASTNode::TYPE returnType = ASTNode::stringToType(m_CurrentToken.identifier);

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::bopen) {
        return parseError<ASTFunctionDefinitionNode>("Syntax Error: Expecting '{' instead of {}", m_CurrentToken);
    }

    std::unique_ptr<ASTBlockNode> body = parseBlock();

    return std::make_unique<ASTFunctionDefinitionNode>(name, std::move(args), returnType, std::move(body));
}

std::unique_ptr<ASTStructDefinitionNode> Parser::parseStructDefintion() {
    parseInfo("struct definition");
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::label) {
        return parseError<ASTStructDefinitionNode>("Sybtax Error: Expecting a label instead of {}", m_CurrentToken);
    }

    std::string name = m_CurrentToken.identifier;

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::bopen) {
        return parseError<ASTStructDefinitionNode>("Syntax Error: Expecting '{' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    std::vector<std::unique_ptr<ASTDeclarationNode>> attributes;
    std::vector<std::unique_ptr<ASTFunctionDefinitionNode>> methods;

    while (m_CurrentToken == token::func || m_CurrentToken == token::type) {
        if (m_CurrentToken == token::type) {
            std::unique_ptr<ASTDeclarationNode> attribute = parseDeclaration();
            attributes.push_back(std::move(attribute));
        } else {
            std::unique_ptr<ASTFunctionDefinitionNode> method = parseFunctionDefinition();
            methods.push_back(std::move(method));
        }

        m_CurrentToken = m_Lexer.getNextToken();
    }

    return std::make_unique<ASTStructDefinitionNode>(name, std::move(attributes), std::move(methods));
}

std::unique_ptr<ASTStructInitializationNode> Parser::parseStructInitialization(std::unique_ptr<ASTIdentifierNode> t_Struct) { parseInfo("struct initialization");
    std::string name = m_CurrentToken.identifier;

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::paropen) {
        return parseError<ASTStructInitializationNode>("Syntax Error: Expecting '(' instead of {}", m_CurrentToken);
    }

    std::vector<std::unique_ptr<ASTExprNode>> attributes;

    m_CurrentToken = m_Lexer.getNextToken();

    while(m_CurrentToken != token::parclose) {
        auto attribute = parseExpr();
        attributes.push_back(std::move(attribute));

        if (m_CurrentToken == token::comma) {
            m_CurrentToken = m_Lexer.getNextToken();
        }
    }

    m_CurrentToken = m_Lexer.getNextToken(); // Eat ')'

    if (m_CurrentToken != token::semicolon) {
        return parseError<ASTStructInitializationNode>("Syntax Error: Expected ';' instead of {}", m_CurrentToken);
    }

    return std::make_unique<ASTStructInitializationNode>(std::move(t_Struct), name, std::move(attributes));
}

std::unique_ptr<ASTStructAssignmentNode> Parser::parseStructAssignement(std::string name) {
    parseInfo("struct assignement");
    m_CurrentToken = m_Lexer.getNextToken();

    std::vector<std::unique_ptr<ASTExprNode>> attributes;

    while (m_CurrentToken != token::bclose) {
        auto expr = parseExpr();
        attributes.push_back(std::move(expr));

        if (m_CurrentToken == token::comma)
            m_CurrentToken = m_Lexer.getNextToken();
    }

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::semicolon) {
        return parseError<ASTStructAssignmentNode>("Syntax Error: Expecting ';' instead of {}", m_CurrentToken);
    }

    return std::make_unique<ASTStructAssignmentNode>(name, std::move(attributes));
}

std::unique_ptr<ASTAttributeAssignmentNode> Parser::parseAttributeAssignment(
        std::string structName, std::string attributeName) {
    parseInfo("attribute assignement");
    m_CurrentToken = m_Lexer.getNextToken();

    auto value = parseExpr();

    return std::make_unique<ASTAttributeAssignmentNode>(structName, attributeName, std::move(value));
}

std::unique_ptr<ASTArrayDefinitionNode> Parser::parseArrayDefinition(ASTNode::TYPE type, std::string name) {
    parseInfo("array definition");
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::int_value) {
        return parseError<ASTArrayDefinitionNode>("Syntax Error: Expected an int instead of {}", m_CurrentToken);
    }

    const size_t size = (size_t)std::stoi(m_CurrentToken.identifier);

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::iclose) {
        return parseError<ASTArrayDefinitionNode>("Syntax Error: Expecting ']' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::eq) {
        return parseArrayInitialization(type, name, size);
    }

    if (m_CurrentToken != token::semicolon) {
        return parseError<ASTArrayDefinitionNode>("Syntax Error: Expecting ';' instead of {}", m_CurrentToken);
    }

    return std::make_unique<ASTArrayDefinitionNode>(name, size, type);
}

std::unique_ptr<ASTArrayInitializationNode> Parser::parseArrayInitialization(ASTNode::TYPE type, std::string name, size_t size) {
    parseInfo("array initialization");
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::iopen) {
        return parseError<ASTArrayInitializationNode>("Syntax Error: Expecting '[' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken(); // Eat '['
    std::vector<std::unique_ptr<ASTExprNode>> values;

    for (int i = 0; i < size; i++) {
        auto expr = parseExpr();
        values.push_back(std::move(expr));

        if (m_CurrentToken != token::comma && i != (size - 1)) {
            return parseError<ASTArrayInitializationNode>("Syntax Error: Expecting ',' instead of {}", m_CurrentToken);
        } else {
            if (m_CurrentToken != token::iclose && i == (size - 1)) {
                return parseError<ASTArrayInitializationNode>("Syntax Error: Expecting ']' instead of {}", m_CurrentToken);
            }
            m_CurrentToken = m_Lexer.getNextToken();
        }
    }

    if (m_CurrentToken != token::semicolon) {
        return parseError<ASTArrayInitializationNode>("Syntax Error: Expecting ';' instead of {}", m_CurrentToken);
    }

    return std::make_unique<ASTArrayInitializationNode>(name, type, size, std::move(values));
}

std::unique_ptr<ASTArrayAssignmentNode> Parser::parseArrayAssignment(std::string name) {
    parseInfo("array assignement");
    m_CurrentToken = m_Lexer.getNextToken();

    std::vector<std::unique_ptr<ASTExprNode>> values;

    while (m_CurrentToken != token::iclose) {
        auto expr = parseExpr();
        values.push_back(std::move(expr));

        if (m_CurrentToken != token::comma && m_CurrentToken != token::iclose) {
            return parseError<ASTArrayAssignmentNode>("Syntax Error: Expecting ',' or ']' instead of {}", m_CurrentToken);
        }

        if (m_CurrentToken == token::comma) {
            m_CurrentToken = m_Lexer.getNextToken();
        }
    }

    return std::make_unique<ASTArrayAssignmentNode>(name, std::move(values));
}

std::unique_ptr<ASTArrayMemeberAssignmentNode> Parser::parseArrayMemberAssignment(std::string name, size_t index) {
    parseInfo("array member assignment");
    m_CurrentToken = m_Lexer.getNextToken();

    auto expr = parseExpr();

    return std::make_unique<ASTArrayMemeberAssignmentNode>(name, index, std::move(expr));
}

std::unique_ptr<ASTExprNode> Parser::parseLabelExpr() {
    parseInfo("label expr");
    std::string identifier = m_CurrentToken.identifier;

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::point) {
        return parseAttributeAccess(identifier);
    }

    if (m_CurrentToken == token::access_sym) {
        auto namespaceIdentifier = parseNamespaceIdentifier(std::move(identifier));

        if (m_CurrentToken == token::paropen) {
            return parseFunctionCall(std::move(namespaceIdentifier));
        }

        return std::move(namespaceIdentifier);
    }

    if (m_CurrentToken == token::paropen) {
        auto identifierNode = std::make_unique<ASTIdentifierNode>(identifier);
        return parseFunctionCall(std::move(identifierNode));
    }

    if (m_CurrentToken == token::iopen) {
        return parseArrayAccess(identifier);
    }

    return std::make_unique<ASTIdentifierNode>(identifier);
}

std::unique_ptr<ASTNode> Parser::parseLabel(std::string identifier) {
    parseInfo("label node");
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::point) {
        return parseAttributeAccessNode(identifier);
    }

    if (m_CurrentToken == token::access_sym) {
        auto namespaceIdentifier = parseNamespaceIdentifier(std::move(identifier));

        if (m_CurrentToken == token::paropen) {
            return parseFunctionCall(std::move(namespaceIdentifier));
        }

        if (m_CurrentToken == token::label) {
            return parseStructInitialization(std::move(namespaceIdentifier));
        }

        return std::move(namespaceIdentifier);
    }

    if (m_CurrentToken == token::paropen) {
        auto identifierNode = std::make_unique<ASTIdentifierNode>(identifier);
        return parseFunctionCall(std::move(identifierNode));
    }

    if (m_CurrentToken == token::iopen) {
        return parseArrayAccessNode(identifier);
    }

    if (m_CurrentToken == token::label) {
        auto structIdentifier = std::make_unique<ASTIdentifierNode>(identifier);
        return parseStructInitialization(std::move(structIdentifier));
    }

    if (m_CurrentToken == token::eq) {
        m_CurrentToken = m_Lexer.getNextToken();
        if (m_CurrentToken == token::iopen) {
            return parseArrayAssignment(identifier);
        }

        if (m_CurrentToken == token::bopen) {
            return parseStructAssignement(identifier);
        }

        return parseAssignment(identifier);
    }

    return std::make_unique<ASTIdentifierNode>(identifier);
}

std::unique_ptr<ASTBinaryNode> Parser::parseBinary(std::unique_ptr<ASTExprNode> lhs) {
    parseInfo("binary");
    Operator t_Operator;

    switch (m_CurrentToken.token) {
        case token::plus:
            t_Operator = Operator::plus;
            break;
        case token::minus:
            t_Operator = Operator::minus;
            break;
        case token::times:
            t_Operator = Operator::times;
            break;
        case token::divide:
            t_Operator = Operator::divide;
            break;
        case token::mod:
            t_Operator = Operator::mod;
            break;
        case token::lth:
            t_Operator = Operator::lth;
            break;
        case token::mth:
            t_Operator = Operator::mth;
            break;
        case token::orsym:
            t_Operator = Operator::orsym;
            break;
        case token::andsym:
            t_Operator = Operator::andsym;
            break;
        case token::eqcomp:
            t_Operator = Operator::eqcomp;
            break;
        case token::leq:
            t_Operator = Operator::leq;
            break;
        case token::meq:
            t_Operator = Operator::meq;
            break;
    }

    m_CurrentToken = m_Lexer.getNextToken();

    auto rhs = parseExpr();

    return std::make_unique<ASTBinaryNode>(std::move(lhs), t_Operator, std::move(rhs));
}

std::unique_ptr<ASTNamespaceIdentifierNode> Parser::parseNamespaceIdentifier(std::string t_Namespace) {
    parseInfo("namespace identifier");
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::label) {
        return parseError<ASTNamespaceIdentifierNode>("Syntax Error: Expecting a label instead of {}", m_CurrentToken);
    }

    std::string identifier = m_CurrentToken.identifier;
    m_CurrentToken = m_Lexer.getNextToken();

    
    return std::make_unique<ASTNamespaceIdentifierNode>(t_Namespace, identifier);
}

std::unique_ptr<ASTFunctionCallNode> Parser::parseFunctionCall(std::unique_ptr<ASTIdentifierNode> callee) {
    parseInfo("function call");
    std::vector<std::unique_ptr<ASTExprNode>> args;

    m_CurrentToken = m_Lexer.getNextToken();

    while (m_CurrentToken != token::parclose) {
        auto arg = parseExpr();
        args.push_back(std::move(arg));

        if (m_CurrentToken != token::comma && m_CurrentToken != token::parclose) {
            return parseError<ASTFunctionCallNode>("Syntax Error: Expecting ',' or ')' instead of {}", m_CurrentToken);
        }

        if (m_CurrentToken == token::comma)
            m_CurrentToken = m_Lexer.getNextToken();
    }

    m_CurrentToken = m_Lexer.getNextToken(); // Eat ')'

    return std::make_unique<ASTFunctionCallNode>(std::move(callee), std::move(args));
}

std::unique_ptr<ASTMethodCallNode> Parser::parseMethodCall(std::string structIdentifier, std::string methodIdentifier) {
    parseInfo("method call");
    m_CurrentToken = m_Lexer.getNextToken();

    std::vector<std::unique_ptr<ASTExprNode>> args;

    while (m_CurrentToken != token::parclose) {
        auto arg = parseExpr();
        args.push_back(std::move(arg));

        if (m_CurrentToken != token::comma && m_CurrentToken != token::parclose) {
            return parseError<ASTMethodCallNode>("Syntax Error: Expecting ',' or ')' instead of {}", m_CurrentToken);
        }

        if (m_CurrentToken == token::comma)
            m_CurrentToken = m_Lexer.getNextToken();
    }

    m_CurrentToken = m_Lexer.getNextToken(); // Eat ')'

    return std::make_unique<ASTMethodCallNode>(structIdentifier, methodIdentifier, std::move(args));
}

std::unique_ptr<ASTAttributeAccessNode> Parser::parseAttributeAccess(std::string structIdentifier) {
    parseInfo("attribute access");
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::label) {
        return parseError<ASTAttributeAccessNode>("Syntax Error: Expecting a label instead of {}", m_CurrentToken);
    }

    std::string attribute = m_CurrentToken.identifier;

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::paropen) {
        return parseMethodCall(structIdentifier, attribute);
    }

    return std::make_unique<ASTAttributeAccessNode>(structIdentifier, attribute);
}

std::unique_ptr<ASTRangeNode> Parser::parseRange(std::unique_ptr<ASTExprNode> expr) {
    parseInfo("range");
    RangeOperator t_Operator;

    switch (m_CurrentToken.token) {
        case token::fromto:
            t_Operator = RangeOperator::ft;
            break;
        case token::fromtol:
            t_Operator = RangeOperator::ftl;
            break;
        case token::fromtominus:
            t_Operator = RangeOperator::ftm;
            break;
        case token::fromoreto:
            t_Operator = RangeOperator::fmt;
            break;
        default:
            return parseError<ASTRangeNode>("Syntax Error: Expected a range operator instead of: {} ", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    auto stop = parseExpr();

    return std::make_unique<ASTRangeNode>(std::move(expr), t_Operator, std::move(stop));
}

std::unique_ptr<ASTExprNode> Parser::parseArrayAccess(std::string name) {
    parseInfo("array access");
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::int_value) {
        return parseError<ASTArrayAccessNode>("Syntax Error: Expecting an int instead of {}", m_CurrentToken);
    }

    size_t index = (size_t)std::stoi(m_CurrentToken.identifier);

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::iclose) {
        return parseError<ASTArrayAccessNode>("Syntax Error: Expecting ']' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::eq) {
        return parseError<ASTExprNode>("This should never happend (array access to assignement)");
    }

    return std::make_unique<ASTArrayAccessNode>(name, index);
}

std::unique_ptr<ASTNode> Parser::parseArrayAccessNode(std::string name) {
    parseInfo("array access node");
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::int_value) {
        return parseError<ASTArrayAccessNode>("Syntax Error: Expecting an int instead of {}", m_CurrentToken);
    }

    size_t index = (size_t)std::stoi(m_CurrentToken.identifier);

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::iclose) {
        return parseError<ASTArrayAccessNode>("Syntax Error: Expecting ']' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::eq) {
        return parseArrayMemberAssignment(name, index);
    }

    return std::make_unique<ASTArrayAccessNode>(name, index);
}

std::unique_ptr<ASTNode> Parser::parseAttributeAccessNode(std::string structIdentifier) {
    parseInfo("attribute access node");
    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken != token::label) {
        return parseError<ASTAttributeAccessNode>("Syntax Error: Expecting a label instead of {}", m_CurrentToken);
    }

    std::string attribute = m_CurrentToken.identifier;

    m_CurrentToken = m_Lexer.getNextToken();

    if (m_CurrentToken == token::paropen) {
        return parseMethodCall(structIdentifier, attribute);
    }

    if (m_CurrentToken == token::eq) {
        return parseAttributeAssignment(structIdentifier, attribute);
    }

    return std::make_unique<ASTAttributeAccessNode>(structIdentifier, attribute);
}

std::unique_ptr<ASTExprNode> Parser::parseParenExpr() {
    parseInfo("parn expr");
    m_CurrentToken = m_Lexer.getNextToken();

    auto expr = parseExpr();

    if (m_CurrentToken != token::parclose) {
        return parseError<ASTExprNode>("Syntax Error: Expecting ')' instead of {}", m_CurrentToken);
    }

    return std::move(expr);
}

std::unique_ptr<ASTAssignmentNode> Parser::parseAssignment(std::string identifier) {
    parseInfo("assignment");
    auto expr = parseExpr();

    if (m_CurrentToken != token::semicolon) {
        return parseError<ASTAssignmentNode>("Syntax Error: Expected ';' instead of {}", m_CurrentToken);
    }

    m_CurrentToken = m_Lexer.getNextToken();

    return std::make_unique<ASTAssignmentNode>(identifier, std::move(expr));
}
