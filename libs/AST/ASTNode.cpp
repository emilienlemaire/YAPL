#include "AST/ASTNode.hpp"

void ASTProgramNode::addNode(std::unique_ptr<ASTNode> node) {
    m_Nodes.push_back(std::move(node));
}
