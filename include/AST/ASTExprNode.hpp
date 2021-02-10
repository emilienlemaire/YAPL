#pragma once

#include "AST/ASTNode.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace yapl {
    using SharedScope = std::shared_ptr<SymbolTable>;

    class ASTExprNode : public ASTNode {
    private:
    public:
        ASTExprNode(SharedScope);
    };
}
