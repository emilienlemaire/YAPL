#include "AST/ASTExprNode.hpp"
#include <memory>
#include <string>
#include <vector>

ASTBinaryNode::ASTBinaryNode(
        std::unique_ptr<ASTExprNode> leftOperrand,
        char t_Operator,
        std::unique_ptr<ASTExprNode> rightOperrand
        )
    : m_LeftOperrand(std::move(leftOperrand)),
      m_Operator(t_Operator),
      m_RightOperrand(std::move(rightOperrand))
{}

ASTIdentifierNode::ASTIdentifierNode(std::string identifier)
    : m_Identifier(std::move(identifier))
{}

ASTNamspaceIdentifierNode::ASTNamspaceIdentifierNode(
        std::string t_Namespace,
        std::string identifier
        )
    : m_Namespace(t_Namespace), m_Identifier(identifier)
{}

ASTFunctionCallNode::ASTFunctionCallNode(
        std::string name,
        std::vector<std::unique_ptr<ASTExprNode>> args
        )
    : m_Name(name), m_Args(std::move(args))
{}

ASTMethodCallNode::ASTMethodCallNode(
        std::string structIdentifier,
        std::string methodName,
        std::vector<std::unique_ptr<ASTExprNode>> args)
    : m_StructIdentifier(structIdentifier), m_MethodName(methodName), m_Args(std::move(args))
{}

