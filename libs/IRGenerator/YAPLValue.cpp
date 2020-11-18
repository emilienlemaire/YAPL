#include "IRGenerator/YAPLValue.hpp"

YAPLType YAPLValue::AstTypeToYAPLType(ASTNode::TYPE type){
    switch (type) {
        case ASTNode::TYPE::NONE:
            return YAPLType::VOID;
        case ASTNode::TYPE::INT:
            return YAPLType::INT;
        case ASTNode::TYPE::DOUBLE:
            return YAPLType::DOUBLE;
        case ASTNode::TYPE::BOOL:
            return YAPLType::BOOL;
        case ASTNode::TYPE::STRING:
            return YAPLType::STRING;
        case ASTNode::TYPE::VOID:
            return YAPLType::VOID;
    }
}
