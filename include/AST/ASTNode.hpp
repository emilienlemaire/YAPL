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

    typedef typename std::vector<std::unique_ptr<ASTNode>> vec_type;
    typedef typename vec_type::iterator iterator;
    typedef typename vec_type::const_iterator const_iterator;

    inline iterator begin() noexcept { return m_Nodes.begin(); }
    inline const_iterator cbegin() const noexcept { return m_Nodes.cbegin(); }
    inline iterator end() noexcept { return m_Nodes.end(); }
    inline const_iterator cend() const noexcept { return m_Nodes.cend(); }
};
