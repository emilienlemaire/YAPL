#pragma once

#include <string>

#include "Type.hpp"

namespace yapl {
    enum class ValueKind {
        Constant,
        Variable,
        Function,
        Type,
        Method
    };
    
    class Value {
    private:
        ValueKind m_Kind;
        std::string m_Name;
        Type m_Type;
    public:
        Value(ValueKind, const std::string&, Type);
    };
}   
