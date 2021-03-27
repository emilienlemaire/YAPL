#include "Symbol/SymbolTable.hpp"
#include <memory>
#include <utility>

namespace yapl {
    std::shared_ptr<SymbolTable> SymbolTable::InitTopSymTab() {
        auto int_t = Type::CreateBaseType("int");
        auto float_t = Type::CreateBaseType("float");
        auto double_t = Type::CreateBaseType("double");
        auto bool_t = Type::CreateBaseType("bool");
        auto char_t = Type::CreateBaseType("char");
        auto void_t = Type::CreateBaseType("void");

        auto int_v = Value::CreateTypeValue("int", int_t);
        auto float_v = Value::CreateTypeValue("float", float_t);
        auto double_v = Value::CreateTypeValue("double", double_t);
        auto bool_v = Value::CreateTypeValue("bool", bool_t);
        auto char_v = Value::CreateTypeValue("char", char_t);
        auto void_v = Value::CreateTypeValue("void", void_t);

        SymbolTable s;

        s.m_ParentScope = nullptr;
        s.m_ChildrenScopes = std::vector<std::shared_ptr<SymbolTable>>();

        s.insert(int_v);
        s.insert(float_v);
        s.insert(double_v);
        s.insert(bool_v);
        s.insert(char_v);
        s.insert(void_v);

        return std::make_shared<SymbolTable>(s);
    }

    bool SymbolTable::insert(std::shared_ptr<Value> val) {
        std::string name = val->getName();

        if (m_Symbols.find(name) != m_Symbols.end()) {
            return false;
        }

        m_Symbols[name] = std::move(val);

        return true;
    }

    std::shared_ptr<Value> SymbolTable::lookup(const std::string &name) {
        auto it = m_Symbols.find(name);

        if (it == m_Symbols.end()) {
            if (m_ParentScope != nullptr) {
                return m_ParentScope->lookup(name);
            }

            return nullptr;
        }

        return it->second;
    }

    std::shared_ptr<SymbolTable> SymbolTable::pushScope(
            std::shared_ptr<SymbolTable> parent
    ) {
        SymbolTable s;
        s.m_ParentScope = std::move(parent);

        auto s_ptr = std::make_shared<SymbolTable>(s);

        m_ChildrenScopes.push_back(s_ptr);

        return s_ptr;
    }

    std::shared_ptr<SymbolTable> SymbolTable::popScope() {
        return m_ParentScope;
    }
} // namespace yapl
