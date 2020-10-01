#pragma once

#include <string>
#include "AST/ASTNode.hpp"

#include "llvm/ADT/StringRef.h"

enum class YAPLType {
    NONE,
    VOID,
    INT,
    DOUBLE,
    BOOL,
    STRING
};

class YAPLValue {
private:
    YAPLType m_Type;
    std::string m_Name;
public:
    static YAPLType AstTypeToYAPLType(ASTNode::TYPE);
    YAPLValue(llvm::StringRef name, YAPLType type)
        : m_Name(name.str()), m_Type(type)
    {}
};
