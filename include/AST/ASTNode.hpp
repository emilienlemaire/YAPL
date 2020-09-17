#pragma once

#include <cstddef>
#include <memory>
#include <vector>

class ASTNode {
public:
    enum TYPE {
        NONE, INT, DOUBLE, BOOL, STRING, VOID
    };

    static TYPE stringToType(std::string str) {
        if (str == "int") {
            return INT;
        } else if (str == "double" || str == "float") {
            return DOUBLE;
        } else if (str == "bool") {
            return BOOL;
        } else if (str == "string") {
            return STRING;
        } else if (str == "void") {
            return VOID;
        }

        return NONE;
    }
};

class ASTProgramNode : public ASTNode {
private:
    std::vector<std::unique_ptr<ASTNode>> m_Nodes;
public:
    ASTProgramNode()
        :m_Nodes(std::vector<std::unique_ptr<ASTNode>>())
    {};
    void addNode(std::unique_ptr<ASTNode> node);
};
