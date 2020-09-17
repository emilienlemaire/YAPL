#pragma once

#include <memory>
#include <vector>

class ASTNode {
public:
    enum TYPE {
        INT, DOUBLE, BOOL, STRING, VOID
    };
};

class ASTProgramNode : public ASTNode {
private:
    std::vector<std::unique_ptr<ASTNode>> m_Nodes;
public:
    explicit ASTProgramNode() = default;
    void addNode(std::unique_ptr<ASTNode> node);
};
