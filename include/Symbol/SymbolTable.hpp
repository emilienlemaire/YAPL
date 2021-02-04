#pragma once

#include <memory>
#include <vector>

#include "parallel_hashmap/phmap.h"
#include "Value.hpp"

namespace yapl {
    class SymbolTable {
    private:
        std::shared_ptr<SymbolTable> m_ParentScope;
        std::vector<std::shared_ptr<SymbolTable>> m_ChildrenScopes;
        phmap::node_hash_map<std::string, std::shared_ptr<Value>> m_Symbols;

    public:
        bool insert(std::shared_ptr<Value> val);
        std::shared_ptr<Value> lookup(std::string);
        std::shared_ptr<SymbolTable> pushScope(std::shared_ptr<SymbolTable>);
        std::shared_ptr<SymbolTable> popScope();

        static std::shared_ptr<SymbolTable> InitTopSymTab();
    };
}
