#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>

#include "AST/ASTNode.hpp"
#include "AST/ASTExprNode.hpp"

class ASTStatementNode : public ASTNode
{};

class ASTImportNode : public ASTStatementNode {
private:
    std::string m_Module;
    std::vector<std::string> m_SubModules;
public:
    ASTImportNode(std::string module, std::vector<std::string> subModules);
    ASTImportNode(std::string module);
};

class ASTExportNode : public ASTStatementNode {
private:
    std::unique_ptr<ASTStatementNode> m_Module;
public:
    ASTExportNode(std::unique_ptr<ASTStatementNode> module);
};

class ASTDeclarationNode : public ASTStatementNode {
private:
    std::string m_Name;
    ASTNode::TYPE m_Type;
public:
    ASTDeclarationNode(std::string name, ASTNode::TYPE type);
    const std::string &getName() const { return m_Name; }
    const ASTNode::TYPE &getType() const { return m_Type; }
};

class ASTInitializationNode : public ASTDeclarationNode {
private:
    std::unique_ptr<ASTExprNode> m_Value;
public:
    ASTInitializationNode(std::string name, ASTNode::TYPE type, std::unique_ptr<ASTExprNode> value)
        : ASTDeclarationNode(name, type), m_Value(std::move(value))
    {}
    ASTExprNode *getValue() const { return m_Value.get(); }
};

class ASTAssignmentNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::unique_ptr<ASTExprNode> m_Value;
public:
    ASTAssignmentNode(std::string name, std::unique_ptr<ASTExprNode> value)
        : m_Name(std::move(name)), m_Value(std::move(value))
    {}

    const std::string &getName() const { return m_Name; }
    ASTExprNode *getValue() const { return m_Value.get(); }
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
    std::unique_ptr<ASTBlockNode> m_IfBlock;
    std::unique_ptr<ASTBlockNode> m_ElseBlock;
public:
    ASTIfNode(
            std::unique_ptr<ASTExprNode> condition,
            std::unique_ptr<ASTBlockNode> ifBlock,
            std::unique_ptr<ASTBlockNode> elseBlock
            );
};

class ASTForNode: public ASTStatementNode {
private:
    std::unique_ptr<ASTDeclarationNode> m_Iterator;
    std::unique_ptr<ASTExprNode> m_Condition;
    std::unique_ptr<ASTBlockNode> m_Block;
public:
    ASTForNode(
            std::unique_ptr<ASTDeclarationNode> iterator,
            std::unique_ptr<ASTExprNode> condition,
            std::unique_ptr<ASTBlockNode> block
            );
};

class ASTFunctionDefinitionNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::vector<std::unique_ptr<ASTDeclarationNode>> m_Args;
    ASTNode::TYPE m_ReturnType;
    std::unique_ptr<ASTBlockNode> m_Body;
public:
    ASTFunctionDefinitionNode(
            std::string name,
            std::vector<std::unique_ptr<ASTDeclarationNode>> args,
            ASTNode::TYPE returnType,
            std::unique_ptr<ASTBlockNode> body
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
    std::unique_ptr<ASTIdentifierNode> m_Struct;
    std::string m_Name;
    std::vector<std::unique_ptr<ASTExprNode>> m_AttributesValues;
public:
    ASTStructInitializationNode(
            std::unique_ptr<ASTIdentifierNode> t_Struct,
            std::string name,
            std::vector<std::unique_ptr<ASTExprNode>> attributesValues
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

class ASTArrayDefinitionNode: public ASTDeclarationNode {
private:
    std::string m_Name;
    ASTNode::TYPE m_Type;
    size_t m_Size;
public:
    ASTArrayDefinitionNode(std::string name, size_t size, ASTNode::TYPE type);
};

class ASTArrayInitializationNode: public ASTArrayDefinitionNode {
private:
    std::vector<std::unique_ptr<ASTExprNode>> m_Values;
public:
    ASTArrayInitializationNode(
            std::string name,
            ASTNode::TYPE type,
            size_t size,
            std::vector<std::unique_ptr<ASTExprNode>> values
            )
        : ASTArrayDefinitionNode(std::move(name), size, type), m_Values(std::move(values))
    {}
};

class ASTArrayAssignmentNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::vector<std::unique_ptr<ASTExprNode>> m_Values;
public:
    ASTArrayAssignmentNode(
            std::string name,
            std::vector<std::unique_ptr<ASTExprNode>> values
            )
        : m_Name(std::move(name)), m_Values(std::move(values))
    {}
};

class ASTArrayMemeberAssignmentNode: public ASTStatementNode {
private:
    std::string m_ArrayName;
    size_t m_Index;
    std::unique_ptr<ASTExprNode> m_Value;
public:
    ASTArrayMemeberAssignmentNode(
            std::string arrayName,
            size_t index,
            std::unique_ptr<ASTExprNode> value);
};

