#include "AST/ASTExprNode.hpp"
#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"
#include "Lexer/Lexer.hpp"
#include <CppLogger2/CppLogger2.h>
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>

class Parser {
private:
    CppLogger::CppLogger m_Logger;
    Lexer m_Lexer;

    std::unique_ptr<ASTProgramNode> m_Program;

    Token m_CurrentToken;

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
    std::unique_ptr<ASTNode> parseNextBlock();
    std::unique_ptr<ASTStatementNode> parseStatement();
    std::unique_ptr<ASTExprNode> parseExpr();
    std::unique_ptr<ASTNode> parseLabel(std::string indentifier);
    std::unique_ptr<ASTExprNode> parseLabelExpr();
    std::unique_ptr<ASTExprNode> parseParenExpr();
    std::unique_ptr<ASTNode> parseArrayAccessNode(std::string);
    std::unique_ptr<ASTNode> parseAttributeAccessNode(std::string);

    // Statement parsing
    std::unique_ptr<ASTImportNode> parseImport();
    std::unique_ptr<ASTExportNode> parseExport();
    std::unique_ptr<ASTDeclarationNode> parseDeclaration();
    std::unique_ptr<ASTInitializationNode> parseInitialization(std::string name, ASTNode::TYPE type);
    std::unique_ptr<ASTAssignmentNode> parseAssignment(std::string);
    std::unique_ptr<ASTReturnNode> parseReturn();
    std::unique_ptr<ASTBlockNode> parseBlock();
    std::unique_ptr<ASTIfNode> parseIf();
    std::unique_ptr<ASTForNode> parseFor();
    std::unique_ptr<ASTFunctionDefinitionNode> parseFunctionDefinition();
    std::unique_ptr<ASTStructDefinitionNode> parseStructDefintion();
    std::unique_ptr<ASTStructInitializationNode> parseStructInitialization(std::unique_ptr<ASTIdentifierNode>);
    std::unique_ptr<ASTStructAssignmentNode> parseStructAssignement(std::string);
    std::unique_ptr<ASTAttributeAssignmentNode> parseAttributeAssignment(std::string, std::string);
    std::unique_ptr<ASTArrayDefinitionNode> parseArrayDefinition(ASTNode::TYPE, std::string);
    std::unique_ptr<ASTArrayInitializationNode> parseArrayInitialization(ASTNode::TYPE, std::string, size_t);
    std::unique_ptr<ASTArrayAssignmentNode> parseArrayAssignment(std::string);
    std::unique_ptr<ASTArrayMemeberAssignmentNode> parseArrayMemberAssignment(std::string, size_t);

    // Expression parsing
    std::unique_ptr<ASTBinaryNode> parseBinary(std::unique_ptr<ASTExprNode>);
    std::unique_ptr<ASTIdentifierNode> parseIdentifier(std::string);
    std::unique_ptr<ASTNamespaceIdentifierNode> parseNamespaceIdentifier(std::string);
    std::unique_ptr<ASTFunctionCallNode> parseFunctionCall(std::unique_ptr<ASTIdentifierNode>);
    std::unique_ptr<ASTMethodCallNode> parseMethodCall(std::string, std::string);
    std::unique_ptr<ASTAttributeAccessNode> parseAttributeAccess(std::string);
    std::unique_ptr<ASTRangeNode> parseRange(std::unique_ptr<ASTExprNode>);
    std::unique_ptr<ASTExprNode> parseArrayAccess(std::string);

    // Templated parsing
    template<typename T>
    std::unique_ptr<ASTLiteralNode<T>> parseLiteral();
    template<> std::unique_ptr<ASTLiteralNode<int>> parseLiteral<int>() {
        int literal = std::stoi(m_CurrentToken.identifier);

        return std::make_unique<ASTLiteralNode<int>>(literal);
    }
    template<> std::unique_ptr<ASTLiteralNode<double>> parseLiteral<double>() {
        double literal = std::stod(m_CurrentToken.identifier);

        return std::make_unique<ASTLiteralNode<double>>(literal);
    }
    template<> std::unique_ptr<ASTLiteralNode<bool>> parseLiteral<bool>() {
        if (m_CurrentToken == token::truelabel) {
            return std::make_unique<ASTLiteralNode<bool>>(true);
        }
        return std::make_unique<ASTLiteralNode<bool>>(false);
    }
    template<> std::unique_ptr<ASTLiteralNode<std::string>> parseLiteral<std::string>() {
        m_Logger.printWarn("String are not implemented yet, please do use them");
        return nullptr;
    }
};
