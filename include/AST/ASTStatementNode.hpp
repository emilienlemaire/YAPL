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
    ASTImportNode(std::string &module, std::vector<std::string> &subModules);
    ASTImportNode(std::string &module);
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
    std::string m_StructName;
public:
    ASTDeclarationNode(std::string &name, ASTNode::TYPE type, const std::string &strcutName = "");
    [[nodiscard]] const std::string &getName() const { return m_Name; }
    [[nodiscard]] const ASTNode::TYPE &getType() const { return m_Type; }
    [[nodiscard]] const std::string &getStructName() const { return m_StructName; }
};

class ASTInitializationNode : public ASTDeclarationNode {
private:
    std::unique_ptr<ASTExprNode> m_Value;
public:
    ASTInitializationNode(std::string name, ASTNode::TYPE type, std::unique_ptr<ASTExprNode> value)
        : ASTDeclarationNode(name, type), m_Value(std::move(value))
    {}
    [[nodiscard]] ASTExprNode *getValue() const { return m_Value.get(); }
};

class ASTAssignmentNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::unique_ptr<ASTExprNode> m_Value;
public:
    ASTAssignmentNode(std::string name, std::unique_ptr<ASTExprNode> value)
        : m_Name(std::move(name)), m_Value(std::move(value))
    {}

    [[nodiscard]] const std::string &getName() const { return m_Name; }
    [[nodiscard]] ASTExprNode *getValue() const { return m_Value.get(); }
};

class ASTReturnNode: public ASTStatementNode {
private:
    std::unique_ptr<ASTExprNode> m_ReturnExpr;
public:
    ASTReturnNode(std::unique_ptr<ASTExprNode> returnExpr);
    [[nodiscard]] ASTExprNode *getExpr() const { return m_ReturnExpr.get(); }
};


class ASTBlockNode: public ASTStatementNode {
private:
    std::vector<std::unique_ptr<ASTNode>> m_Nodes;

    using baseIterator = std::vector<std::unique_ptr<ASTNode>>::iterator;
    using baseConstIterator = std::vector<std::unique_ptr<ASTNode>>::const_iterator;
public:
    ASTBlockNode(std::vector<std::unique_ptr<ASTNode>> nodes);

    [[nodiscard]] baseIterator begin() { return m_Nodes.begin(); }
    [[nodiscard]] baseIterator end()   { return m_Nodes.end(); }
    [[nodiscard]] baseConstIterator cbegin() const { return m_Nodes.cbegin(); }
    [[nodiscard]] baseConstIterator cend() const   { return m_Nodes.cend(); }

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
    [[nodiscard]] ASTExprNode *getCond() const { return m_Condition.get(); }
    [[nodiscard]] ASTBlockNode *getThen() const { return m_IfBlock.get(); }
    [[nodiscard]] ASTBlockNode *getElse() const { return m_ElseBlock.get(); }
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
    [[nodiscard]] ASTDeclarationNode *getDecl() const { return m_Iterator.get(); }
    [[nodiscard]] ASTExprNode *getCond() const { return m_Condition.get(); }
    [[nodiscard]] ASTBlockNode *getBlock() const { return m_Block.get(); }
};

class ASTFunctionDefinitionNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::vector<std::unique_ptr<ASTDeclarationNode>> m_Args;
    ASTNode::TYPE m_ReturnType;
    std::unique_ptr<ASTBlockNode> m_Body;
    std::string m_ReturnStruct;
public:
    ASTFunctionDefinitionNode(
            std::string &name,
            std::vector<std::unique_ptr<ASTDeclarationNode>> args,
            ASTNode::TYPE returnType,
            std::unique_ptr<ASTBlockNode> body,
            std::string returnStruct = ""
            );

    [[nodiscard]] const std::string &getName() const { return m_Name; }
    [[nodiscard]] std::vector<std::unique_ptr<ASTDeclarationNode>> getArgs() { return std::move(m_Args); }
    [[nodiscard]] const ASTNode::TYPE &getType() const { return m_ReturnType; }
    [[nodiscard]] ASTBlockNode *getBody() const { return m_Body.get(); }
    [[nodiscard]] const std::string &getReturnStructName() const { return m_ReturnStruct; }
};

class ASTStructDefinitionNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::vector<std::unique_ptr<ASTDeclarationNode>> m_Attributes;
    std::vector<std::unique_ptr<ASTFunctionDefinitionNode>> m_Methods;
public:
    ASTStructDefinitionNode(
            std::string &name,
            std::vector<std::unique_ptr<ASTDeclarationNode>> attributes,
            std::vector<std::unique_ptr<ASTFunctionDefinitionNode>> methods
            );
    [[nodiscard]] const std::string &getName() const { return m_Name; }
    [[nodiscard]] std::vector<std::unique_ptr<ASTDeclarationNode>> getAttributes() { return std::move(m_Attributes); }
    [[nodiscard]] std::vector<std::unique_ptr<ASTFunctionDefinitionNode>> getMethods() { return std::move(m_Methods); }
};

class ASTStructInitializationNode: public ASTStatementNode {
private:
    std::unique_ptr<ASTIdentifierNode> m_Struct;
    std::string m_Name;
    std::vector<std::unique_ptr<ASTExprNode>> m_AttributesValues;
public:
    ASTStructInitializationNode(
            std::unique_ptr<ASTIdentifierNode> t_Struct,
            std::string &name,
            std::vector<std::unique_ptr<ASTExprNode>> attributesValues
            );
    [[nodiscard]] ASTIdentifierNode *getStruct() const { return m_Struct.get(); }
    [[nodiscard]] const std::string &getName() const { return m_Name; }
    [[nodiscard]] std::vector<std::unique_ptr<ASTExprNode>> getAttributesValues() { return std::move(m_AttributesValues); }
};

class ASTStructAssignmentNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::vector<std::unique_ptr<ASTExprNode>> m_AttributesValues;
public:
    ASTStructAssignmentNode(
            std::string &name,
            std::vector<std::unique_ptr<ASTExprNode>> attributesValues
            );
    [[nodiscard]] std::vector<std::unique_ptr<ASTExprNode>> getAttributesValues() { return std::move(m_AttributesValues); }
    [[nodiscard]] const std::string &getName() const { return m_Name; }
};

class ASTAttributeAssignmentNode: public ASTStatementNode {
private:
    std::string m_StrcutName;
    std::string m_AttributeName;
    std::unique_ptr<ASTExprNode> m_Value;
public:
    ASTAttributeAssignmentNode(
            std::string &strcutName,
            std::string &attributeName,
            std::unique_ptr<ASTExprNode> value
            );
    [[nodiscard]] const std::string &getStructName() const { return m_StrcutName; }
    [[nodiscard]] const std::string &getAttributeName() const { return m_AttributeName; }
    [[nodiscard]] ASTExprNode *getValue() const { return m_Value.get(); }
};

class ASTArrayDefinitionNode: public ASTDeclarationNode {
private:
    const size_t m_Size;
public:
    ASTArrayDefinitionNode(std::string &name, size_t size, ASTNode::TYPE type);
    [[nodiscard]] const size_t &getSize() const { return m_Size; }
};

class ASTArrayInitializationNode: public ASTArrayDefinitionNode {
private:
    std::vector<std::unique_ptr<ASTExprNode>> m_Values;
    
    using baseIt = std::vector<std::unique_ptr<ASTExprNode>>::iterator;
    using baseConstIt = std::vector<std::unique_ptr<ASTExprNode>>::const_iterator;
public:
    ASTArrayInitializationNode(
            std::string &name,
            ASTNode::TYPE type,
            size_t size,
            std::vector<std::unique_ptr<ASTExprNode>> values
            )
        : ASTArrayDefinitionNode(name, size, type), m_Values(std::move(values))
    {}
    [[nodiscard]] baseIt begin() { return m_Values.begin(); }
    [[nodiscard]] baseIt end()   { return m_Values.end(); }
    [[nodiscard]] const baseConstIt cbegin() const { return m_Values.cbegin(); }
    [[nodiscard]] const baseConstIt cend()   const { return m_Values.cend(); }
};

class ASTArrayAssignmentNode: public ASTStatementNode {
private:
    std::string m_Name;
    std::vector<std::unique_ptr<ASTExprNode>> m_Values;
    using baseType = std::vector<std::unique_ptr<ASTExprNode>>;
    using baseIt = baseType::iterator;
    using baseConstIt = baseType::const_iterator;
public:
    ASTArrayAssignmentNode(
            std::string &name,
            std::vector<std::unique_ptr<ASTExprNode>> values
            )
        : m_Name(std::move(name)), m_Values(std::move(values))
    {}
    [[nodiscard]] const std::string &getName() const { return m_Name; }
    [[nodiscard]] baseIt begin() { return m_Values.begin(); }
    [[nodiscard]] baseIt end()   { return m_Values.end(); }
    [[nodiscard]] const baseConstIt cbegin() const { return m_Values.cbegin(); }
    [[nodiscard]] const baseConstIt cend()   const { return m_Values.cend(); }
    [[nodiscard]] const size_t getSize() const { return m_Values.size(); }
};

class ASTArrayMemeberAssignmentNode: public ASTStatementNode {
private:
    std::string m_ArrayName;
    size_t m_Index;
    std::unique_ptr<ASTExprNode> m_Value;
public:
    ASTArrayMemeberAssignmentNode(
            std::string &arrayName,
            size_t index,
            std::unique_ptr<ASTExprNode> value);
    [[nodiscard]] const std::string &getName() const { return m_ArrayName; }
    [[nodiscard]] const size_t &getIndex() const { return m_Index; }
    [[nodiscard]] ASTExprNode *getValue() const { return m_Value.get(); }
};

