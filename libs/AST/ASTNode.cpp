#include "AST/ASTNode.hpp"
#include <memory>

namespace yapl
{
    ASTNode::ASTNode(std::shared_ptr<SymbolTable> scope)
        : m_Scope(scope)
    {}

    std::shared_ptr<SymbolTable> ASTNode::getScope() const {
        return m_Scope;
    }

    void ASTProgramNode::addNode(std::unique_ptr<ASTNode> node) {
        m_Nodes.push_back(std::move(node));
    }
}
