#include "AST/ASTNode.hpp"
#include <memory>

namespace yapl
{
    void ASTProgramNode::addNode(std::unique_ptr<ASTNode> node) {
        m_Nodes.push_back(std::move(node));
    }
    
    std::shared_ptr<SymbolTable> ASTNode::getScope() const {
        return m_Scope;
    }
}
