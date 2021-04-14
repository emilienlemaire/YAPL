/**
 * include/Symbol/SymbolTable.hpp
 * Copyright (c) 2021 Emilien Lemaire <emilien.lem@icloud.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <memory>
#include <vector>

#include "parallel_hashmap/phmap.h"
#include "Value.hpp"

// FIXME: FIX EVERYTHING
namespace yapl {
    class SymbolTable {
    private:
        std::shared_ptr<SymbolTable> m_ParentScope;
        std::vector<std::shared_ptr<SymbolTable>> m_ChildrenScopes;
        phmap::node_hash_map<std::string, std::shared_ptr<Value>> m_Symbols;

    public:
        bool insert(std::shared_ptr<Value> val);
        std::shared_ptr<Value> lookup(const std::string&);
        std::shared_ptr<SymbolTable> pushScope(std::shared_ptr<SymbolTable>);
        std::shared_ptr<SymbolTable> popScope();

        static std::shared_ptr<SymbolTable> InitTopSymTab();
    };
} // namespace yapl
