#pragma once

#include <memory>
#include <vector>
class ASTExprNode
{};

template<typename T>
class ASTLiteralNode: public ASTExprNode {
private:
    T m_Value;
public:
    ASTLiteralNode(T value)
        : m_Value(value)
    {}
};

class ASTBinaryNode: public ASTExprNode {
private:
    std::unique_ptr<ASTExprNode> m_LeftOperrand;
    char m_Operator;
    std::unique_ptr<ASTExprNode> m_RightOperrand;
public:
    ASTBinaryNode(
            std::unique_ptr<ASTExprNode> leftOperrand,
            char t_Operator,
            std::unique_ptr<ASTExprNode> rightOperrand
            );
};

class ASTIdentifierNode: public ASTExprNode {
private:
    std::string m_Identifier;
public:
    ASTIdentifierNode(std::string identifier);
};

class ASTNamspaceIdentifierNode: public ASTExprNode {
private:
    std::string m_Namespace;
    std::string m_Identifier;
public:
    ASTNamspaceIdentifierNode(std::string t_Namespace, std::string identifier);
};

class ASTFunctionCallNode: public ASTExprNode {
private:
    std::string m_Name;
    std::vector<std::unique_ptr<ASTExprNode>> m_Args;
public:
    ASTFunctionCallNode(std::string name, std::vector<std::unique_ptr<ASTExprNode>> args);
};

class ASTMethodCallNode: public ASTExprNode {
private:
    std::string m_StructIdentifier;
    std::string m_MethodName;
    std::vector<std::unique_ptr<ASTExprNode>> m_Args;
public:
    ASTMethodCallNode(
            std::string structIdentifier,
            std::string methodName,
            std::vector<std::unique_ptr<ASTExprNode>> args
            );
};

