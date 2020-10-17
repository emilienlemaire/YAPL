#pragma once

#include "AST/ASTNode.hpp"
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Parser;

enum class RangeOperator {
    ft,
    ftl,
    ftm,
    fmt
};

enum class Operator {
    plus,
    minus,
    times,
    divide,
    mod,
    lth,
    mth,
    orsym,
    andsym,
    eqcomp,
    leq,
    meq,
    neq
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
    T getValue() const { return m_Value; }
};

class ASTBinaryNode: public ASTExprNode {
private:
    std::unique_ptr<ASTExprNode> m_LeftOperrand;
    Operator m_Operator;
    std::unique_ptr<ASTExprNode> m_RightOperrand;

    friend Parser;
public:
    ASTBinaryNode(
            std::unique_ptr<ASTExprNode> leftOperrand,
            Operator t_Operator,
            std::unique_ptr<ASTExprNode> rightOperrand
            );

    std::unique_ptr<ASTExprNode> getLeftOperrand() { return std::move(m_LeftOperrand); }
    std::unique_ptr<ASTExprNode> getRightOperrand() { return std::move(m_RightOperrand); }
    Operator getOperator() const { return m_Operator; }
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
    const std::string &getName() const { return m_Identifier; }
};

class ASTNamespaceIdentifierNode: public ASTIdentifierNode {
private:
    std::string m_Namespace;
public:
    ASTNamespaceIdentifierNode(std::string t_Namespace, std::string identifier);
};

class ASTFunctionCallNode: public ASTExprNode {
private:
    std::unique_ptr<ASTIdentifierNode> m_Name;
    std::vector<std::unique_ptr<ASTExprNode>> m_Args;
public:
    ASTFunctionCallNode(std::unique_ptr<ASTIdentifierNode> name, std::vector<std::unique_ptr<ASTExprNode>> args);
};

class ASTAttributeAccessNode : public ASTExprNode {
private:
    std::string m_Name;
    std::string m_Attribute;
public:
    ASTAttributeAccessNode(std::string name, std::string attribute)
        : m_Name(std::move(name)), m_Attribute(std::move(attribute))
    {}
    const std::string &getName() const { return m_Name; }
    const std::string &getAttribute() const { return m_Attribute; }
};

class ASTMethodCallNode: public ASTAttributeAccessNode {
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

class ASTArrayAccessNode : public ASTExprNode {
private:
    std::string m_Name;
    size_t m_Index;
public:
    ASTArrayAccessNode(std::string name, size_t index)
        : m_Name(std::move(name)), m_Index(index)
    {}
    const std::string &getName() const { return m_Name; }
    const size_t &getIndex() const { return m_Index; }
};
