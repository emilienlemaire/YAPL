#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <string>

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

    virtual ~ASTNode() = default;
};

class ASTProgramNode : public ASTNode {
private:
    std::vector<std::unique_ptr<ASTNode>> m_Nodes;
public:
    ASTProgramNode(std::vector<std::unique_ptr<ASTNode>> nodes)
        :m_Nodes(std::move(nodes))
    {};
    void addNode(std::unique_ptr<ASTNode> node);

    using vec_type = std::vector<std::unique_ptr<ASTNode>>;
    using iterator = vec_type::iterator;
    using const_iterator = vec_type::const_iterator;

    [[nodiscard]] inline iterator begin() noexcept { return m_Nodes.begin(); }
    [[nodiscard]] inline const_iterator cbegin() const noexcept { return m_Nodes.cbegin(); }
    [[nodiscard]] inline iterator end() noexcept { return m_Nodes.end(); }
    [[nodiscard]] inline const_iterator cend() const noexcept { return m_Nodes.cend(); }
};
