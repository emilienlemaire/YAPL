#include "AST/ASTExprNode.hpp"
#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"
#include "CppLogger2/include/CppLogger.h" //NOLINT
#include "Lexer/Lexer.hpp"
#include <CppLogger2/CppLogger2.h>
#include <cstddef>
#include <memory>

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

public:
    Parser(std::string file="", CppLogger::Level level=CppLogger::Level::Warn);
    std::unique_ptr<ASTNode> parseNext();
    void parse();

private:
    std::unique_ptr<ASTStatementNode> parseStatement();
    std::unique_ptr<ASTExprNode> parseExpr();
    std::unique_ptr<ASTNode> parseLabel(std::string indentifier);

    // Statement parsing
    std::unique_ptr<ASTImportNode> parseImport();
    std::unique_ptr<ASTExportNode> parseExport();
    std::unique_ptr<ASTDeclarationNode> parseDeclaration();
    std::unique_ptr<ASTInitializationNode> parseInitialization(std::string name, ASTNode::TYPE type);
    std::unique_ptr<ASTReturnNode> parseReturnNode();
    std::unique_ptr<ASTBlockNode> parseBlock();
    std::unique_ptr<ASTIfNode> parseIf();
    std::unique_ptr<ASTForNode> parseFor();
    std::unique_ptr<ASTFunctionDefinitionNode> parseFunctionDefinition();
    std::unique_ptr<ASTStructDefinitionNode> parseStructDefintion();
    std::unique_ptr<ASTStructInitializationNode> parseStructInitialization();
    std::unique_ptr<ASTStructAssignmentNode> parseStructAssignement();
    std::unique_ptr<ASTAttributeAssignmentNode> parseAttributeAssignment();
    std::unique_ptr<ASTArrayDefinitionNode> parseArrayDefinition(ASTNode::TYPE, std::string);
    std::unique_ptr<ASTArrayMemeberAssignmentNode> parseArrayMemberAssignment();

    // Expression parsing
    std::unique_ptr<ASTBinaryNode> parseBinary();
    std::unique_ptr<ASTIdentifierNode> parseIdentifier();
    std::unique_ptr<ASTNamspaceIdentifierNode> parseNamespaceIdentifier();
    std::unique_ptr<ASTFunctionCallNode> parseFunctionCall();
    std::unique_ptr<ASTMethodCallNode> parseMethodeCall();
    std::unique_ptr<ASTRangeNode> parseRange();

    // Templated parsing
    // Statement
    template<size_t t_Size>
    std::unique_ptr<ASTArrayInitializationNode<t_Size>> parseArrayInitialization(std::string);
    template<size_t t_Size>
    std::unique_ptr<ASTArrayAssignmentNode<t_Size>> parseArrayAssignment(std::string);
    // Expression
    template<typename T>
    std::unique_ptr<ASTLiteralNode<T>> parseLiteral();
};
