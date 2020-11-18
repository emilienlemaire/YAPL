#include "AST/ASTExprNode.hpp"
#include <memory>
#include <string>
#include <vector>

ASTBinaryNode::ASTBinaryNode(
        std::unique_ptr<ASTExprNode> leftOperrand,
        Operator t_Operator,
        std::unique_ptr<ASTExprNode> rightOperrand
        )
    : m_LeftOperrand(std::move(leftOperrand)),
      m_Operator(t_Operator),
      m_RightOperrand(std::move(rightOperrand))
{}

ASTIdentifierNode::ASTIdentifierNode(std::string identifier)
    : m_Identifier(std::move(identifier))
{}

ASTNamespaceIdentifierNode::ASTNamespaceIdentifierNode(
        std::string t_Namespace,
        std::string identifier
        )
    : m_Namespace(t_Namespace), ASTIdentifierNode(identifier)
{}

ASTFunctionCallNode::ASTFunctionCallNode(
        std::unique_ptr<ASTIdentifierNode> name,
        std::vector<std::unique_ptr<ASTExprNode>> args
        )
    : m_Name(std::move(name)), m_Args(std::move(args))
{}

ASTMethodCallNode::ASTMethodCallNode(
        std::string structIdentifier,
        std::string methodName,
        std::vector<std::unique_ptr<ASTExprNode>> args)
    : ASTAttributeAccessNode(std::move(structIdentifier), std::move(methodName)), m_Args(std::move(args))
{}

