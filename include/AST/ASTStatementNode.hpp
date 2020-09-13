#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>

#include "AST/ASTNode.hpp"
#include "AST/ASTExprNode.hpp"

class ASTStatementNode
{};

class ASTImportNode : public ASTStatementNode {
private:
    std::string m_Module;
    std::vector<std::string> m_SubModules;
public:
    ASTImportNode(std::string module, std::vector<std::string> subModules);
};

class ASTExportNode : public ASTStatementNode {
private:
    std::vector<std::unique_ptr<ASTStatementNode>> m_Modules;
public:
    ASTExportNode(std::vector<std::unique_ptr<ASTStatementNode>> modules);
};

class ASTDeclarationNode : public ASTStatementNode {
private:
    std::string m_Name;
    ASTNode::TYPE m_Type;
public:
    ASTDeclarationNode(std::string name, ASTNode::TYPE type);
};

template<typename T>
class ASTInitializationNode : public ASTStatementNode {
private:
    std::string m_Name;
    ASTNode::TYPE m_Type;
    T m_Value;
public:
    ASTInitializationNode(std::string name, ASTNode::TYPE type, T value)
        : m_Name(std::move(name)), m_Type(type), m_Value(value)
    {}
};

template<typename T>
class ASTAssignmentNode: public ASTStatementNode {
private:
    std::string m_Name;
    ASTNode::TYPE m_Type;
    T m_Value;
public:
    ASTAssignmentNode(std::string name, ASTNode::TYPE type, T value)
        : m_Name(std::move(name)), m_Type(type), m_Value(value)
    {}
};

class ASTReturnNode: public ASTStatementNode {
private:
    std::unique_ptr<ASTExprNode> m_ReturnExpr;
public:
    ASTReturnNode(std::unique_ptr<ASTExprNode> returnExpr);
};


class ASTBlockNode: public ASTStatementNode {
private:
    std::vector<std::unique_ptr<ASTNode>> m_Nodes;
public:
    ASTBlockNode(std::vector<std::unique_ptr<ASTNode>> nodes);
};

class ASTIfNode: public ASTStatementNode {
private:
    std::unique_ptr<ASTExprNode> m_Condition;
    std::unique_ptr<ASTBlockNode> m_Block;
public:
    ASTIfNode(std::unique_ptr<ASTExportNode> condition, std::unique_ptr<ASTBlockNode> block);
};

class ASTElseNode: public ASTStatementNode {
private:
    std::unique_ptr<ASTIfNode> m_IfStatement;
    std::unique_ptr<ASTBlockNode> m_Block;
public:
    ASTElseNode(std::unique_ptr<ASTIfNode> ifStatement, std::unique_ptr<ASTBlockNode> block);
};

class ASTElseIfNode: public ASTStatementNode {
private:
    std::unique_ptr<ASTIfNode> m_IfStatement;
    std::vector<std::unique_ptr<ASTElseIfNode>> m_ElseIfNodes;
    std::unique_ptr<ASTExprNode> m_Condition;
    std::unique_ptr<ASTBlockNode> m_Block;
public:
    ASTElseIfNode(
            std::unique_ptr<ASTIfNode> ifStatement,
            std::vector<std::unique_ptr<ASTElseIfNode>> elseIfNodes,
            std::unique_ptr<ASTExprNode> condition,
            std::unique_ptr<ASTBlockNode> block
            );
};

class ASTForNode: public ASTStatementNode {
private:
    std::string m_Iterator;
    std::unique_ptr<ASTExprNode> m_Condition;
    std::unique_ptr<ASTBlockNode> m_Block;
public:
    ASTForNode(
            std::string iterator,
            std::unique_ptr<ASTExprNode> condition,
            std::unique_ptr<ASTBlockNode> block
            );
};

class ASTFunctionDefinitionNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::vector<ASTDeclarationNode> m_Args;
    ASTNode::TYPE m_ReturnType;
    std::unique_ptr<ASTBlockNode> m_Body;
public:
    ASTFunctionDefinitionNode(
            std::string name,
            std::vector<ASTDeclarationNode> args,
            ASTNode::TYPE returnType,
            std::unique_ptr<ASTBlockNode> m_Body
            );
};

class ASTStructDefinitionNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::vector<std::unique_ptr<ASTDeclarationNode>> m_Attributes;
    std::vector<std::unique_ptr<ASTFunctionDefinitionNode>> m_Methods;
public:
    ASTStructDefinitionNode(
            std::string name,
            std::vector<std::unique_ptr<ASTDeclarationNode>> attributes,
            std::vector<std::unique_ptr<ASTFunctionDefinitionNode>> methods
            );
};

class ASTStructInitializationNode: public ASTStatementNode {
private:
    std::string m_Struct;
    std::string m_Name;
    std::vector<std::unique_ptr<ASTExprNode>> m_AttributesValues;
public:
    ASTStructInitializationNode(
            std::string structName,
            std::string name,
            std::vector<std::unique_ptr<ASTExportNode>> attributesValues
            );
};

class ASTStructAssignmentNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::vector<std::unique_ptr<ASTExprNode>> m_AttributesValues;
public:
    ASTStructAssignmentNode(
            std::string name,
            std::vector<std::unique_ptr<ASTExprNode>> attributesValues
            );
};

class ASTAttributeAssignmentNode: public ASTStatementNode {
private:
    std::string m_StrcutName;
    std::string m_AttributeName;
    std::unique_ptr<ASTExprNode> m_Value;
public:
    ASTAttributeAssignmentNode(
            std::string strcutName,
            std::string attributeName,
            std::unique_ptr<ASTExprNode> value
            );
};

class ASTArrayDefinitionNode: public ASTStatementNode {
private:
    std::string m_Name;
    size_t m_Size;
    ASTNode::TYPE m_Type;
public:
    ASTArrayDefinitionNode(std::string name, size_t size, ASTNode::TYPE type);
};

template<size_t t_Size>
class ASTArrayInitializationNode: public ASTStatementNode {
private:
    std::string m_Name;
    ASTNode::TYPE m_Type;
    std::array<std::unique_ptr<ASTExprNode>, t_Size> m_Values;
public:
    ASTArrayInitializationNode(
            std::string name,
            ASTNode::TYPE type,
            std::array<std::unique_ptr<ASTExprNode>, t_Size> values
            )
        : m_Name(std::move(name)), m_Type(type), m_Values(std::move(values))
    {}
};

template<size_t t_Size>
class ASTArrayAssignmentNode: ASTStatementNode {
private:
    std::string m_Name;
    std::array<std::unique_ptr<ASTExprNode>, t_Size> m_Values;
public:
    ASTArrayAssignmentNode(
            std::string name,
            std::array<std::unique_ptr<ASTExprNode>, t_Size> values
            )
        : m_Name(std::move(name)), m_Values(std::move(values))
    {}
};

class ASTArrayMemeberAssignmentNode: public ASTStatementNode {
private:
    std::string m_ArrayName;
    size_t m_Index;
public:
    ASTArrayMemeberAssignmentNode(std::string arrayName, size_t index);
};

