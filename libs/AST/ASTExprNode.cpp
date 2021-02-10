#include <memory>
#include <string>
#include <vector>

#include "AST/ASTExprNode.hpp"

namespace yapl {
    ASTExprNode::ASTExprNode(SharedScope scope)
        : ASTNode(scope)
    {}
}
