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
        std::shared_ptr<Type> m_Type;
        Value();
        Value(ValueKind, const std::string&, Type);
    public:
        static std::shared_ptr<Value> CreateTypeValue(const std::string&, std::shared_ptr<Type>);

        std::string getName() const { return m_Name; }
    };
}   
