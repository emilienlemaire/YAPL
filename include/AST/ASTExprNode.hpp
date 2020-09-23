#pragma once

#include "AST/ASTNode.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

enum class RangeOperator {
    ft,
    ftl,
    ftm,
    fmt
};

class ASTExprNode : public ASTNode
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

class ASTRangeNode: public ASTExprNode {
private:
    std::unique_ptr<ASTExprNode> m_Start;
    RangeOperator m_Operator;
    std::unique_ptr<ASTExprNode> m_Stop;
public:
    ASTRangeNode(
        std::unique_ptr<ASTExprNode> start,
        RangeOperator t_Operator,
        std::unique_ptr<ASTExprNode> stop
        )
    : m_Start(std::move(start)), m_Operator(t_Operator), m_Stop(std::move(stop))
    {}
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

