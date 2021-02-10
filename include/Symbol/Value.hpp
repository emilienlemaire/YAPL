#pragma once

#include <string>
#include <vector>

#include "Type.hpp"

namespace yapl {
    class SymbolTable;

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
        std::vector<std::shared_ptr<Value>> m_Params;
        std::shared_ptr<SymbolTable> m_Scope = nullptr;

        Value() = default;
        Value(ValueKind, const std::string&, Type);
    public:
        static std::shared_ptr<Value> CreateTypeValue(const std::string&, std::shared_ptr<Type>);

        std::string getName() const { return m_Name; }
    };
}   
