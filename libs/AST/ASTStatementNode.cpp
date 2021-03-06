#include "AST/ASTStatementNode.hpp"
#include "AST/ASTExprNode.hpp"
#include "AST/ASTNode.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

ASTImportNode::ASTImportNode(std::string &module, std::vector<std::string> &subModules)
    : m_Module(std::move(module)), m_SubModules(std::move(subModules))
{}

ASTImportNode::ASTImportNode(std::string &module)
    : m_Module(std::move(module))
{}

ASTExportNode::ASTExportNode(std::unique_ptr<ASTStatementNode> module)
    : m_Module(std::move(module))
{}

ASTDeclarationNode::ASTDeclarationNode(std::string &name, ASTNode::TYPE type,
        const std::string &structName)
    : m_Name(std::move(name)), m_Type(type), m_StructName(std::move(structName))
{}

ASTReturnNode::ASTReturnNode(std::unique_ptr<ASTExprNode> returnExpr)
    : m_ReturnExpr(std::move(returnExpr))
{}

ASTBlockNode::ASTBlockNode(std::vector<std::unique_ptr<ASTNode>> nodes)
    : m_Nodes(std::move(nodes))
{}

ASTIfNode::ASTIfNode(
        std::unique_ptr<ASTExprNode> condition,
        std::unique_ptr<ASTBlockNode> ifBlock,
        std::unique_ptr<ASTBlockNode> elseBlock
        )
    : m_Condition(std::move(condition)),
      m_IfBlock(std::move(ifBlock)),
      m_ElseBlock(std::move(elseBlock))
{}

ASTForNode::ASTForNode(
        std::unique_ptr<ASTDeclarationNode> iterator,
        std::unique_ptr<ASTExprNode> condition,
        std::unique_ptr<ASTBlockNode> block
        )
    : m_Iterator(std::move(iterator)), m_Condition(std::move(condition)), m_Block(std::move(block))
{}

ASTFunctionDefinitionNode::ASTFunctionDefinitionNode(
        std::string &name,
        std::vector<std::unique_ptr<ASTDeclarationNode>> args,
        ASTNode::TYPE returnType,
        std::unique_ptr<ASTBlockNode> body,
        std::string structReturn)
    : m_Name(std::move(name)), m_Args(std::move(args)), m_ReturnType(returnType),
        m_Body(std::move(body)), m_ReturnStruct(structReturn)
{}

ASTStructDefinitionNode::ASTStructDefinitionNode(
        std::string &name,
        std::vector<std::unique_ptr<ASTDeclarationNode>> attributes,
        std::vector<std::unique_ptr<ASTFunctionDefinitionNode>> methods)
    : m_Name(std::move(name)), m_Attributes(std::move(attributes)), m_Methods(std::move(methods))
{}

ASTStructInitializationNode::ASTStructInitializationNode(
        std::unique_ptr<ASTIdentifierNode> t_Struct,
        std::string &name,
        std::vector<std::unique_ptr<ASTExprNode>> attributesValues)
    : m_Struct(std::move(t_Struct)), m_Name(std::move(name)), m_AttributesValues(std::move(attributesValues))
{}

ASTStructAssignmentNode::ASTStructAssignmentNode(
        std::string &name,
        std::vector<std::unique_ptr<ASTExprNode>> attributesValues)
    : m_Name(std::move(name)), m_AttributesValues(std::move(attributesValues))
{}

ASTAttributeAssignmentNode::ASTAttributeAssignmentNode(
        std::string &structName,
        std::string &attributeName,
        std::unique_ptr<ASTExprNode> value)
    : m_StrcutName(std::move(structName)),
      m_AttributeName(std::move(attributeName)),
      m_Value(std::move(value))
{}

ASTArrayDefinitionNode::ASTArrayDefinitionNode(std::string &name, size_t size, ASTNode::TYPE type)
    : ASTDeclarationNode(name, type), m_Size(size)
{}

ASTArrayMemeberAssignmentNode::ASTArrayMemeberAssignmentNode(
        std::string &name,
        size_t index,
        std::unique_ptr<ASTExprNode> value
        )
    : m_ArrayName(std::move(name)), m_Index(index), m_Value(std::move(value))
{}

