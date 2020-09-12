#pragma once

#include <vector>

class ASTNode {
public:
    enum TYPE {
        INT, DOUBLE, BOOL, STRING, VOID
    };
};

class ASTProgramNode : public ASTNode {
private:
    std::vector<ASTNode*> m_Nodes;
public:
    explicit ASTProgramNode(std::vector<ASTNode*>);
};
